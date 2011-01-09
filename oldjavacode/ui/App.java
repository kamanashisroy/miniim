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

/**
 * 
 * @author Kamanashis Roy (kamanashisroy@gmail.com)
 */

package net.ayaslive.miniim.ui;

import javax.microedition.lcdui.Alert;
import javax.microedition.lcdui.AlertType;
import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;
import javax.microedition.midlet.MIDlet;
import javax.microedition.midlet.MIDletStateChangeException;

import org.kxml2.kdom.TestDocument;

import net.ayaslive.miniim.common.Account;
import net.ayaslive.miniim.common.IMDocument;
import net.ayaslive.miniim.ui.core.Window;
import net.ayaslive.miniim.utils.WebUtils;
import net.ayaslive.miniim.utils.io.resource.ResourceFetcher;
import net.ayaslive.miniim.utils.simplelogger.SimpleLogger;

public final class App extends MIDlet implements CommandListener {
	private static App self = null;

	public App() {
		WebUtils.createAgent(this);
		self = this;
		Window.dis = Display.getDisplay(this);
	}
	
	public void destroyApp(boolean unconditional) {
		// #mdebug
		try {
		// #enddebug
			Account.destroyAll();
			destroyUI();
			/*! \todo exit smoothlly (close all the background threads) */
			notifyDestroyed();
			ResourceFetcher.getInstance().doDestroy();
		// #mdebug
		} catch(Throwable t) {
			t.printStackTrace();
		}
		// #enddebug
	}
	
	private void destroyUI() {
		MiniWebControler.doDestroy();
		Window.destroyApp();
	}

	protected void pauseApp() {
	}

	/**
	 * Startup step
	 */
	private int step = 0;
	private String stepDesc = "Initiating";
	private Canvas canvas = null;
	
	protected void startApp() throws MIDletStateChangeException {
		try {
			
			// #debug debug
			TestDocument.test();
			
			canvas = new Canvas() {
				protected void paint(Graphics g) {
					final int height = getHeight();
					final int width = getWidth();
					final int halfHeight = height/6;

					// clear
					g.setColor(0x204A87); // blue
					g.fillRect(0, 0, width, halfHeight);
					
					g.setColor(0xFFFFFF); // white
					g.fillRect(0, halfHeight, width, height - halfHeight);
					
					// fill step%
					g.setColor(0xF57900); // orange
					g.fillRect( 0, halfHeight, width*step/100, 2);
					
					// description ..
					g.setColor(0xC35C00); // dark orange
					g.setFont(Font.getFont(Font.FACE_SYSTEM, Font.STYLE_PLAIN, Font.SIZE_SMALL));
					g.drawString(stepDesc, width/2, halfHeight + 10, Graphics.TOP|Graphics.HCENTER);
				}
			};
			
			Window.dis.setCurrent(canvas);
			canvas.setFullScreenMode(true);
			// wait 1 second to let it be full screen ..
			Thread.sleep(1000);
			
			progress(5, "ui");
			Window.init(canvas.getWidth(), canvas.getHeight());
			
			// Loading logging system and accounts
			progress(10, "accounts");
			// start logger
			SimpleLogger.getInstance("http://devel.ayaslive.net/?q=mlog/6/add", "bWluaWltOnBhbmdv", 1000, SimpleLogger.DEBUG, null);
			SimpleLogger.debug(this, "Agent:" + WebUtils.getAgent());
			Account.loadAccounts();
			
			// #ifdef TEST_DIGEST_MD5
//@			WebUtils.getMd5DigestResponse("nonce=\"1693422881\",qop=\"auth\",charset=utf-8,algorithm=md5-sess", "kamanashisroy", "abcdefabcdef", "AUTHENTICATE", "hermes.jabber.org");
			// #endif
			
			// loading Account list dialog
			progress(20, "account ui");
			AccountListDialog.singleton = new AccountListDialog();
			
			// loading Buddy list dialog
			progress(30, "contacts");
			IMDocument.init();
			ContactList.singleton = new ContactList();
	
			// loading conversation dialog
			progress(40, "conversation");
			ConversationDialog.singleton = new ConversationDialog();
			
			// set the listeners
			// progress(45, "Setting Listeners");
			Account.setBuddyStateListener(ContactList.singleton);
			Account.setChangeListener(AccountListDialog.singleton);
			Account.setMessageListener(ConversationDialog.singleton);

			progress(60, "miniweb");
			MiniWebControler.singleton = new MiniWebControler();
			// #ifdef LOG_VIEWER
			SimpleLogger.getRootLogger().setListener(MiniWebControler.singleton);
			// #endif

			// let miniweb load the home page
			progress(70, "home");
			Thread.sleep(1000);
			progress(75, "home .");
			Thread.sleep(1000);
			progress(75, "home ..");
			Thread.sleep(1000);
			
			// load alert system
			progress(80, "L.. alerts");
			singleAlert = new Alert("MiniIM");
			YES_POSITIVE = new Command(Vocabulary.YES, Command.OK, 1);
			NO_POSITIVE = new Command(Vocabulary.NO, Command.CANCEL, 1);
			YES_NEGATIVE = new Command(Vocabulary.YES, Command.CANCEL, 1);
			NO_NEGATIVE = new Command(Vocabulary.NO, Command.OK, 1);
			OK_CMD = new Command(Vocabulary.OK, Command.BACK, 1);
			
			// Now change the view ..
			progress(90, "Done");
			AccountListDialog.singleton.show();
			canvas = null;
			
		} catch(Throwable t) {
			t.printStackTrace();
			progress(0, t.getMessage());
			try {
				Thread.sleep(5000);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
	}
	
	private void progress(int step, String stepDesc) {
		this.step = step;
		this.stepDesc = stepDesc;
		canvas.repaint();
		canvas.serviceRepaints();
	}
	
	protected static void destroy() {
		self.destroyApp(false);
	}
	
	/*! @name Alert system */
	/*@{*/
	private static Alert singleAlert = null;
	private static Command YES_POSITIVE = null;
	private static Command NO_POSITIVE = null;
	private static Command YES_NEGATIVE = null;
	private static Command NO_NEGATIVE = null;
	private static Command OK_CMD = null;
	
	private static Displayable prevDisplay = null;
	private static CommandListener wrappedListener = null;
	public void commandAction(Command cmd, Displayable dis) {
		if(prevDisplay != null) {
			Window.dis.setCurrent(prevDisplay);
			Window.getCurrent().show();
		}
		prevDisplay = null;
		if(wrappedListener != null) {
			wrappedListener.commandAction(cmd, dis);
		}
		wrappedListener = null;
	}
	
	final static void showDialog(String title, String msg, AlertType type, int timeout) {
		synchronized(singleAlert) {
			singleAlert.setTitle(title);
			singleAlert.removeCommand(YES_POSITIVE);
			singleAlert.removeCommand(YES_NEGATIVE);
			singleAlert.removeCommand(NO_POSITIVE);
			singleAlert.removeCommand(NO_NEGATIVE);
			singleAlert.addCommand(OK_CMD);
			singleAlert.setString(msg);
			singleAlert.setType(type);
			wrappedListener = null;
			singleAlert.setCommandListener(self);
			singleAlert.setTimeout(timeout);
			//MainCanvas.singleAlert.setImage(null);
			singleAlert.setIndicator(null);
			prevDisplay = Window.dis.getCurrent();
			Window.dis.setCurrent(singleAlert);
		}
	}
	
	final static void showYesNoDialog(String title, String msg, boolean positive, CommandListener lis) {
		synchronized(singleAlert) {
			singleAlert.setTitle(title);
			singleAlert.removeCommand(YES_POSITIVE);
			singleAlert.removeCommand(YES_NEGATIVE);
			singleAlert.removeCommand(NO_POSITIVE);
			singleAlert.removeCommand(NO_NEGATIVE);
			singleAlert.removeCommand(OK_CMD);
			if(positive) {
				singleAlert.addCommand(YES_POSITIVE);
				singleAlert.addCommand(NO_POSITIVE);
			} else {
				singleAlert.addCommand(YES_NEGATIVE);
				singleAlert.addCommand(NO_NEGATIVE);
			}
			singleAlert.setString(msg);
			singleAlert.setType(AlertType.ALARM);
			wrappedListener = lis;
			singleAlert.setCommandListener(self);
			singleAlert.setTimeout(Alert.FOREVER);
			//MainCanvas.singleAlert.setImage(null);
			singleAlert.setIndicator(null);
			prevDisplay = Window.dis.getCurrent();
			Window.dis.setCurrent(singleAlert);
		}
	}
	/*@}*/
}
