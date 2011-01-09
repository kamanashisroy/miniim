/*
 * This file part of MiniIM.
 *
 * Copyright (C) 2007  Kamanashis Roy
 *
 * MiniIM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MiniIM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MiniIM.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/*
 * ConversationDialog.java
 *
 *     --------------------------
 *     | Conversation- anony..  |
 *     |------------------------|
 *     |                        |
 *     | Bla                    |
 *     | Bla                    |
 *     | ....                   |
 *     | ....                   |
 *     | ....                   |
 *     | ....                   |
 *     --------------------------
 *
 *  Previous,Write	       Next,Back
 *
 * Created on April 24, 2007, 10:48 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 * 
 */

package net.ayaslive.miniim.ui;

import java.io.IOException;
import java.util.Enumeration;

import javax.microedition.lcdui.Image;
import javax.microedition.lcdui.TextField;
import javax.microedition.rms.InvalidRecordIDException;
import javax.microedition.rms.RecordEnumeration;
import javax.microedition.rms.RecordStoreNotOpenException;

import org.kxml2.kdom.Element;

import net.ayaslive.miniim.common.Account;
import net.ayaslive.miniim.common.Contact;
import net.ayaslive.miniim.common.IMDocument;
import net.ayaslive.miniim.common.IMMessage;
import net.ayaslive.miniim.common.MessageListener;
import net.ayaslive.miniim.ui.core.ActionListener;
import net.ayaslive.miniim.ui.core.List;
import net.ayaslive.miniim.ui.core.TextEdit;
import net.ayaslive.miniim.ui.core.TextEditListener;
import net.ayaslive.miniim.ui.core.Window;
import net.ayaslive.miniim.ui.core.list.ListItem;
import net.ayaslive.miniim.ui.core.list.ListItemFactory;
import net.ayaslive.miniim.utils.simplelogger.SimpleLogger;

/**
 *
 * \todo add previous and next conversation window events
 * @author Kamanashis Roy
 */
final class ConversationDialog extends List implements ActionListener,
		MessageListener, TextEditListener, Enumeration {
	private final static String WRITE = "pad";

	private final static String BUDDY_LIST = "Back";
	
	private final static String DETAILS = "Info";
	
	private final static String CHAT = "chat - ";

	private Element bd = null;

	/** single conversation window */
	static ConversationDialog singleton = null;
	
	private final Image img_pending;
	
	private RecordEnumeration en = null;
	
	private final String[] leftMenuOption = {DETAILS};

	/** Creates a new instance of ConversationDialog */
	ConversationDialog() throws IOException {
		super(CHAT, WRITE);
		/* add the commands */
		setActionListener(this);
		img_pending = Image.createImage("/images/message-pending.png");
	}

	protected final void showConversation(Element bd) {
		this.bd = bd;
		title = CHAT + bd.getAttributeValue(Contact.ALIAS);
		reload();
		super.show(BUDDY_LIST, leftMenuOption);
	}

	public final void performAction(String command) {
		if (command == WRITE) {
			TextEdit.getInstance().showTextEdit(this, bd.getAttributeValue(Contact.ALIAS), "", 50, TextField.ANY | TextField.NON_PREDICTIVE);
		} else if (command == BUDDY_LIST) {
			ContactList.show();
		} else if (command == DETAILS) {
			BuddyDialog.singleton.show(bd);
		}
	}

	public synchronized void messageReceived(Account acc, IMMessage mess) {
		if(mess.type == IMMessage.ERROR) {
			/*! \todo show error dialog */
			Window.pushBalloon("Error sending message to " + mess.from.getAttributeValue(Contact.ALIAS), null/* show error image */);
		} else {
			// avoid notification if we are in this conversation ..
			if(!mess.from.equals(bd) || !this.isShowing()) {
				Window.pushBalloon(mess.from.getAttributeValue(Contact.ALIAS), img_pending);
			} else {
				reload();
			}
		}
		repaint();
	}
	
	protected String getHint() {
		return WRITE;
	}

	public synchronized void messageSent(Account acc, IMMessage mess) {
		if(mess.to.equals(bd)) {
			reload();
		}
	}
	
	private synchronized final void reload() {
		if(en != null) {
			en.destroy();
			en = null;
		}
		try {
			en = IMMessage.select(this.bd);
		} catch (RecordStoreNotOpenException e) {
			e.printStackTrace();
		}
		if(en == null || !en.hasNextElement()) {
			if(en != null) {
				en.destroy();
			}
			en = null;
		} else {
			// scroll to highest position ..
			setSelectedIndex(en.numRecords() - 1);
		}
		// System.gc();
		repaint();
	}
	
	public void textEdited(String newText) {
		/* now send the message to the body */
		if (bd == null) {
			SimpleLogger.debug(this, "textEdited()\t\tbd cannot be null");
			return;
		}
		// XXX what to do with subject !!
		IMDocument.sendMessage(bd, null, newText);
		show();
	}

	protected int getCount() {
		if(en == null) {
			return 0;
		}
		en.reset();
		System.out.println("Enumeration is not null : " + en.numRecords());
		return en.numRecords();
	}

	protected Enumeration getItems() {
		// return bd.getConversations();
		return this;
	}

	protected ListItem getListItem(Object obj) {
		return ListItemFactory.createLabel(obj.toString(), null);
	}
	
	/// @name enumeration implementation
	/*@{*/
	public boolean hasMoreElements() {
		if(en == null) {
			return false;
		}
		return en.hasNextElement();
	}

	public Object nextElement() {
		if(en == null) {
			return null;
		}
		
		try {
			IMMessage msg = new IMMessage();
			msg.load(en.nextRecordId());
			return msg;
		} catch (InvalidRecordIDException e) {
			e.printStackTrace();
		}
		return null;
	}
	/*@}*/
}
