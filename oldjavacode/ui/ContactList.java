package net.ayaslive.miniim.ui;

import java.io.IOException;

import javax.microedition.lcdui.Alert;
import javax.microedition.lcdui.AlertType;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Image;

import net.ayaslive.miniim.common.BuddyStateListener;
import net.ayaslive.miniim.common.Contact;
import net.ayaslive.miniim.common.IMDocument;
import net.ayaslive.miniim.ui.core.ActionListener;
import net.ayaslive.miniim.ui.core.Window;
import net.ayaslive.miniim.ui.markup.extended.DefaultComplexListener;

import org.kxml2.kdom.Document;
import org.kxml2.kdom.Element;

final class ContactList implements BuddyStateListener {

	static ContactList singleton = null;
	
	/** Image resources */
    private Image img_avail = null;

    private Image img_unavail = null;

    private Image img_busy = null;

    private Image img_away = null;

    private Image img_xa = null;

    private Image img_group = null;
    
    ContactList() throws Exception {
        img_avail = Image.createImage("/images/avail.png");
        // img_busy = ImageUtils.replaceColor(img_avail, ImageUtils.RED_GREEN);
        img_busy = Image.createImage("/images/busy.png");
        // img_unavail = ImageUtils.replaceColor(img_avail, ImageUtils.AVERAGE);
        img_unavail = Image.createImage("/images/unavail.png");
        // img_away = ImageUtils.replaceColor(img_avail, ImageUtils.GREEN_BLUE | ImageUtils.BLUE_YELLOW);
        img_away = Image.createImage("/images/away.png");
        // img_xa = ImageUtils.replaceColor(img_avail, ImageUtils.GREEN_OTHER);
        img_xa = Image.createImage("/images/xa.png");
        img_group = Image.createImage("/images/group.png");
        
        prepareContactList();
    }
    
    protected final Image getStatusIcon(String status) {
        return status.equals(Contact.AVAILABLE)?img_avail:status.equals(Contact.DND)?img_busy:status.equals(Contact.AWAY)?img_away:status.equals(Contact.UNAVAILABLE)?img_unavail:status.equals(Contact.XA)?img_xa:img_unavail;
    }

	private static void prepareContactList() throws Exception {
		Document groups = new Document();
		groups.load(DefaultComplexListener.MINI_WEB, IMDocument.GROUP_URL,
				false);

		// get the groups
		Element elem, ls = groups.getRootElement().getElement(null, "ls");
		int groupCount = ls.getChildCount();

		for (int i = groupCount; i > 0; i++) {

			// clear all the group children
			elem = (Element) ls.getChild(i);
			elem.removeChildren();

			// see if the group is expanded
			if (DefaultComplexListener.isPositiveAttribute(elem, "e")) {

				// add children to the expanded group
				Document doc = new Document();
				doc
						.searchPrefix(IMDocument.STORE, false,
								IMDocument.CONTACT_PREFIX
										+ elem.getAttributeValue("l"));
				// TODO fill me
			}
		}
		groups.store(DefaultComplexListener.MINI_WEB, IMDocument.GROUP_URL,
				false, 100000);
	}

	protected static void show() {
		// TODO set the url ..
		MiniWebControler.show();
	}

	private void dirty() {
		// TODO fill me
	}

	/** \defgroup #BuddyStateListener implementation */
	/*\@{*/
	public void buddyAdded(Element bd) {
		dirty();
	}

	public void buddyAvailabilityChanged(Element evt) {
		
		// get the availability status
		String available = evt.getAttributeValue(Contact.AVAILABLE);
		
		// we are only dealing with available and unavailable status
		int avail = Contact.AVAILABLE.equals(available)?1:(Contact.UNAVAILABLE.equals(available)?0:-1);
		if(avail != -1) {
			
			// notify the user
			Window.pushBalloon(evt.getAttributeValue(Contact.ALIAS), (avail == 1)?img_avail:img_unavail);
		}
		dirty();
	}

	public void buddyRemoved(Element bd) {
		dirty();
	}

	public void subscribed(Element bd) {
		App.showDialog(bd.toString(), "Remote user " + bd
				+ " has added you on his/her contact list.", AlertType.INFO,
				Alert.FOREVER);
	}

	/** \defgroup Subscription management */
	/*\@{*/
	private Element subscribing = null;

	public void subscriptionRequested(Element from, String reason) {
		/*
		 * show a display asking permission from the user to add a buddy and
		 * response with subscribed
		 */
		if (subscribing != null) {
			/* ! \todo schedule for future dispatch */
			return;
		}
		subscribing = from;
		/*
		 * YesNoDialog.getInstance() .show( "Remote user " + bd +
		 * " requested you to add him in your contact list. Do you want to add him/her ?"
		 * + " He is saying \"" + reason + "\"" , buddy_add_listener);
		 */
//		App
//				.showYesNoDialog(
//						subscribing.toString(),
//						"Remote user "
//								+ subscribing
//								+ " requested you to add him in your contact list. Do you want to add him/her ?"
//								+ ((reason != null) ? (" He is saying \""
//										+ reason + "\"") : ""), true, this);
	}

	public void unsubscribed(Element bd) {
		App.showDialog(bd.toString(), "Remote user " + bd
				+ " has removed you from his/her contact list.",
				AlertType.INFO, Alert.FOREVER);
	}

	public void unsubscriptionRequested(Element bd) {
		App
				.showDialog(
						bd.toString(),
						"Remote user "
								+ bd
								+ " has been removed at his/her request from your contact list.",
						AlertType.INFO, Alert.FOREVER);
	}
	/*\@}*/
	/*\@}*/

//	/**
//	 * Command listener
//	 */
//	public void commandAction(Command cmd, Displayable dis) {
//		if (subscribing != null) {
//
//			// We are listening to subscription question ..
//			if (cmd.getCommandType() == Command.OK) {
//
//				// The user has accepted the subscription ..
//				
//				IMDocument.subscribe(subscribing);
//			}
//			subscribing = null;
//		} else {
//
//			// We are deleting the buddy
//			if (cmd.getCommandType() == Command.CANCEL) {
//
//				// User forced to delete the buddy ..
//				((Buddy) getSelected()).requestRemove();
//			}
//		}
//		this.show();
//	}
//
//	/**
//	 * command listener for canvas commands
//	 */
//	public void performAction(String command) {
//		Element bd;
//		if (command == Vocabulary.PLACES) {
//			MiniWebControler.show();
//			return;
//		}
//
//		if (command == Vocabulary.NEW) {
//			BuddyDialog.singleton.show();
//		}
//
//		if (getSelectedIndex() == markedPos) {
//			int groupCount = IMDocument.getGroups();
//			if (groupCount == 0) {
//				return; // no groups to toggle ..
//			}
//			Enumeration en = BuddyStore.getGroups();
//			if (selectedGroup.equals(Contact.ONLINE)) {
//				selectedGroup = (String) en.nextElement();
//				needsQuickReload();
//				return;
//			}
//			while (en.hasMoreElements()) {
//				String group = (String) en.nextElement();
//				if (selectedGroup.equals(group)) {
//					if (en.hasMoreElements()) {
//						selectedGroup = (String) en.nextElement();
//						needsQuickReload();
//						return;
//					} else {
//						selectedGroup = Contact.ONLINE;
//						needsQuickReload();
//						return;
//					}
//				}
//			}
//			SimpleLogger.debug(this, "Unreachable statement");
//			return;
//		}
//
//		if (command == CHAT) {
//			bd = (Element) getSelected();
//			if (bd != null) {
//				ConversationDialog.singleton.showConversation(bd);
//			}
//		} else if (command == DETAILS) {
//			bd = (Element) getSelected();
//			if (bd != null) {
//				BuddyDialog.singleton.show(bd);
//			}
//		} else if (command == Vocabulary.DELETE) {
//			bd = (Element) getSelected();
//			if (bd == null) {
//				return;
//			}
//			/* confirm from the user */
//			/* YesNoDialog.getInstance().show(, delete_listener); */
//			App
//					.showYesNoDialog(bd.toString(),
//							"Are you sure, you want to delete " + bd + "?",
//							false, this);
//		}/*
//		 * else if (command == OKDialog.OK) { BuddyList.getInstance().show(); }
//		 */
//	}

}
