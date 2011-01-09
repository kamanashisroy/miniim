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
 * AccountDialog.java
 *
 * Created on March 31, 2007, 3:51 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package net.ayaslive.miniim.ui;

import java.util.Enumeration;

import javax.microedition.lcdui.Image;
import javax.microedition.lcdui.TextField;

import org.kxml2.kdom.Element;

import net.ayaslive.miniim.common.Account;
import net.ayaslive.miniim.common.Contact;
import net.ayaslive.miniim.common.IMDocument;
import net.ayaslive.miniim.ui.core.ActionListener;
import net.ayaslive.miniim.ui.core.List;
import net.ayaslive.miniim.ui.core.list.ListItem;
import net.ayaslive.miniim.ui.core.list.ListItemFactory;
import net.ayaslive.miniim.ui.core.TextEdit;
import net.ayaslive.miniim.ui.core.TextEditListener;

/**
 * Input Buddy name and alias OR show buddy information.
 * @author Kamanashis Roy kamanashisroy@gmail.com
 * 
 * \todo add boddy remove and chat commands here
 */
final class BuddyDialog extends List implements ActionListener, TextEditListener, Enumeration {
	/**
	 * name input box
	 */
	private String name;

	/** alias input box */
	private String alias;
	
	/** status */
	private Image status;

	/** account field */
	private Account account;
	
	/** Subscription status */
	private String subscription;

	
//	private final AccountChooser chooser = new AccountChooser();

	/** processing flag */
	private boolean editing = false;
	
	/** The single instance of this class */
	protected final static BuddyDialog singleton = new BuddyDialog();
	
	private Element contact = null;
	
	private int index = -1;
	
	private final String[] leftMenuOption = {Vocabulary.CHAT, Vocabulary.CANCEL};

	/** Creates a new instance of BuddyDialog */
	private BuddyDialog() {
		super("Contact", Vocabulary.EDIT);
		setActionListener(this);
	}

	/** show a new buddy creation box */
	public final void show() {
		/* we are not editing existing buddy */
		editing = false;
		/* keep the buddy name empty */
		/* keep the alias empty */
		/* select the previously selected account */
		account = Account.getAccountAt(0);
		subscription = getSubscriptionString(Contact.NONE);
		/* set the status to subscribe */
		status = ContactList.singleton.getStatusIcon(Contact.UNAVAILABLE);
		super.show(Vocabulary.OK, leftMenuOption);
	}
	
	/** show an existing buddy details */
	protected final void show(Element bd) {
		if(bd == null ) {
			return;
		}
		contact = bd;
		/* set the fields */
		name = bd.getAttributeValue(Contact.ID);
		alias = bd.getAttributeValue(Contact.ALIAS);
		account = IMDocument.getAccount(bd);
		status = ContactList.singleton.getStatusIcon(bd.getAttributeValue(Contact.AVAILABLE));
		subscription = getSubscriptionString(bd.getAttributeValue(Contact.SUBSCRIPTION));
		/*e = bd.keys();
		while(e.hasMoreElements()) {
			key = e.nextElement();
			value = bd.get(key);
			properties += key+":"+value+"\n";
		}*/
		/* we are in editing mode */
		editing = true;
		super.show(Vocabulary.OK, leftMenuOption);
	}

	private final static String getSubscriptionString(String subscription) {
		return subscription.equals(Contact.BOTH)?"BOTH":subscription.equals(Contact.NONE)?"NONE":subscription.equals(Contact.TO)?"TO":subscription.equals(Contact.FROM)?"FROM":"ERROR";
	}
	
	/**
	 * command listener for canvas commands
	 */
	public void performAction(String action) {
		if (action == Vocabulary.OK) { /* handle OK event */
			/* set the acc */
			if(editing) {
				reset();
				ContactList.show();
				return;
			}
			/* request server to add the buddy */
			account.inviteBuddy(name, alias);
			reset();
			ContactList.show();
		} else if (action == Vocabulary.CANCEL) { /* handle Cancel event */
			/* close the account dialog */
			reset();
			ContactList.show();
		} else if (action == Vocabulary.CHAT) {
			if(contact != null) {
				ConversationDialog.singleton.showConversation(contact);
				reset();
			}
		} else if(action == Vocabulary.EDIT) {
			if(editing) {
				/*! \todo show only properties */
			} else {
				switch(getSelectedIndex()) {
				case 0:
					TextEdit.getInstance().showTextEdit(this, "Contact Name", (name == null) ? "" : name, 50, TextField.ANY | TextField.NON_PREDICTIVE);
					break;
				case 1:
					TextEdit.getInstance().showTextEdit(this, "Contact Alias", (alias == null) ? "" : alias, 50, TextField.ANY | TextField.NON_PREDICTIVE);
					break;
				case 3:
//					chooser.show(account);
					// TODO toggle ..
					break;
				}
			}
		}
	}
	
	/**
	 * go to the old displayable
	 */
	private final void reset() {
		contact = null;
		name = null;
		alias = null;
		account = null;
		subscription = null;
		status = null;
	}
	
	protected int getCount() {
		return editing?6:5;
	}

	protected Enumeration getItems() {
		index = -1;
		return this;
	}
	
	public boolean hasMoreElements() {
		return editing? index<5 : index<4;
	}
	
	public Object nextElement() {
		index++;
		switch (index) {
		case 0:
			return ListItemFactory.createTextInput("Name", name);
		case 1:
			return ListItemFactory.createTextInput("Alias", alias);
		case 2:
			return ListItemFactory.createLabel("Status", status);
		case 3:
			/*! \todo This should be list selection of connected Accounts */
			return ListItemFactory.createSelectionBox("Account", account.toString(), !editing);
		case 4:
			return ListItemFactory.createTextInput("Subscription", subscription);
		case 5:
			return ListItemFactory.createLabel("View properties > ", null);
		}
		return null;
	}

	protected ListItem getListItem(Object obj) {
		return (ListItem)obj;
	}
	
	protected String getHint() {
		return Vocabulary.EDIT;
	}
	
	public void textEdited(String newText) {
		switch(getSelectedIndex()) {
		case 0:
			name = newText;
			break;
		case 1:
			alias = newText;
			break;
		}
		super.show();
	}
}
