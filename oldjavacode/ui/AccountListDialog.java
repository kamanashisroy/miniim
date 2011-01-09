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
 * AccountListDialog.java
 *
 * Created on April 1, 2007, 3:37 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package net.ayaslive.miniim.ui;

import java.io.IOException;
import java.util.Enumeration;

import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Image;

import net.ayaslive.miniim.common.Account;
import net.ayaslive.miniim.common.IMProtocol;
import net.ayaslive.miniim.common.IMStateChangeListener;
import net.ayaslive.miniim.ui.core.ActionListener;
import net.ayaslive.miniim.ui.core.List;
import net.ayaslive.miniim.ui.core.Window;
import net.ayaslive.miniim.ui.core.list.ListItem;
import net.ayaslive.miniim.ui.core.list.ListItemFactory;
import net.ayaslive.miniim.utils.io.resource.ResourceIdentifier;
import net.ayaslive.miniim.utils.simplelogger.SimpleLogger;

/**
 * @author Kamanashis Roy
 */
final class AccountListDialog extends List implements ActionListener,
		IMStateChangeListener, Enumeration {
	/** The single instance of the account list dialog */
	static AccountListDialog singleton = null;

	/** Image resources */
	private Image img_connected = null;

	private Image img_connecting = null;

	private Image img_preconnect = null;
	
	private final String[] leftMenuOption = {Vocabulary.CONNECT, Vocabulary.DISCONNECT, Vocabulary.NEW, Vocabulary.DELETE};
	
	private Account editingAcc = null;
	
	private final StringBuffer strBuff = new StringBuffer();
	
	private final CommandListener deleteListener = new CommandListener() {
		public void commandAction(Command cmd, Displayable dis) {
			if (cmd.getCommandType() == Command.CANCEL) {
				Account acc = (Account)getSelected();
				if(acc != null) {
					acc.deleteInstance();
				}
			}
		}
	};

	/** Creates a new instance of AccountListDialog */
	AccountListDialog() throws IOException {
		super("Accounts", Vocabulary.EDIT);
		img_connected = Image.createImage("/images/connected.png");
		img_connecting = Image.createImage("/images/connecting.png");
		img_preconnect = Image.createImage("/images/disconnected.png");
		setActionListener(this);
	}
	
	private final Image statusToImage(int status) {
		switch (status) {
		case IMProtocol.CONNECTED:
			return img_connected;
		case IMProtocol.CONNECTING:
		case IMProtocol.DISCONNECTING:
			return img_connecting;
		case IMProtocol.ERROR:
		case IMProtocol.PRECONNECT:
		default:
			return img_preconnect;
		}
	}

	/** Show new account dialog */
	/*@{*/
	private final void showNewAccountDialog() {
		/* create a blank account */
		editingAcc = Account.getInstance();
		/* set this account to account dialog editing account */
		MiniWebControler.singleton.pushWrapper(new ResourceIdentifier(null, "res://im/AccountDialog.xml"), false);
		MiniWebControler.show();
	}
	/*@}*/

	public final void show() {
		// clear old editing account reference ..
		editingAcc = null;
		if(Account.getCount() == 0) {
			showNewAccountDialog();
			return;
		}
		super.show(Vocabulary.PLACES, leftMenuOption);
	}
	
	Account getAccount() {
		return editingAcc;
	}

	/** ActionListener Implementation */
	/*@{*/
	public final void performAction(String command) {
		if (command == Vocabulary.NEW) {
			showNewAccountDialog();
		} else if (command == Vocabulary.PLACES) {
			MiniWebControler.show();
		} else {
			Account acc = (Account) getSelected();
			if(acc == null) {
				return;
			}
			if (command == Vocabulary.EDIT) {
				editingAcc = acc;
				MiniWebControler.singleton.pushWrapper(new ResourceIdentifier(null, "res://im/AccountDialog.xml"), false);
				MiniWebControler.show();
			} else if (command == Vocabulary.CONNECT) {
				// start logging ..
//				SimpleLogger.getRootLogger().setLevel(SimpleLogger.DEBUG);
//				SimpleLogger.getRootLogger().start();
				acc.connect();
			} else if (command == Vocabulary.DISCONNECT) {
				acc.disconnect();
			} else if (command == Vocabulary.DELETE) {
				/* confirm from the user */
				App.showYesNoDialog(acc.toString(), "Are you sure, you want to delete " + acc + "?", false, deleteListener);
			}
		}
	}
	/*@}*/

	/** List implementation */
	/*@{*/
	protected ListItem getListItem(Object obj) {
		return ListItemFactory.createLabel(obj.toString(), statusToImage(((Account) obj).getConnectionStatus()));
	}
	
	protected String getHint() {
		return Vocabulary.EDIT;
	}

	protected Enumeration getItems() {
		count = 0;
		return this;
	}

	protected int getCount() {
		return Account.getCount();
	}
	/*@}*/
	
	/** IMStateChangeListener implementation */
	/*@{*/
	public final void changed(Account acc) {
		SimpleLogger.debug(this, "changed()newState");
		int newState = acc.getConnectionStatus();
		
		if(this.isShowing()) {
			// repaint if this is showing
			repaint();
		// TODO: repaint if buddylist is showing and the account has gone offline
//		} else if(ContactList.singleton.isShowing() && (newState == IMProtocol.DISCONNECTED || newState == IMProtocol.DISCONNECTING || newState == IMProtocol.ERROR)) {
//			// repaint if buddylist is showing and the account has gone offline
//			ContactList.singleton.repaint();
		}
		// strBuff.setLength(0);
		if(newState == IMProtocol.CONNECTING) {
			strBuff.append(acc.getProgress());
			strBuff.append("% ");
		} else if(newState == IMProtocol.ERROR) {
			strBuff.append("Error ");
		}
		strBuff.append(acc.getDomain());
		
		Window.pushBalloon(strBuff.toString()
				, (
					(newState == IMProtocol.DISCONNECTED)?img_preconnect
					:(
						(newState == IMProtocol.CONNECTED)?img_connected
						:(
							(newState == IMProtocol.CONNECTING)?img_connecting
							:null /* TODO add image here */
						)
					)
				)
				, acc.hashCode(), 1000);
		strBuff.setLength(0);
	}
	/*@}*/

	/// Enumeration implementation
	/*@{*/
	private int count = 0;
	public boolean hasMoreElements() {
		if(count < Account.getCount()) {
			return true;
		}
		return false;
	}

	public Object nextElement() {
		return Account.getAccountAt(count++);
	}
	/*@}*/
}
