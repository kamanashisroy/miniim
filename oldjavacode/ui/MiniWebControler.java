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

package net.ayaslive.miniim.ui;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.EOFException;
import java.io.IOException;
import java.util.Enumeration;
import java.util.Hashtable;

import javax.microedition.io.ContentConnection;
import javax.microedition.lcdui.Image;
import javax.microedition.rms.RecordEnumeration;
import javax.microedition.rms.RecordStore;
import javax.microedition.rms.RecordStoreException;
import javax.microedition.rms.RecordStoreFullException;
import javax.microedition.rms.RecordStoreNotFoundException;
import javax.microedition.rms.RecordStoreNotOpenException;

import org.kxml2.kdom.Document;
import org.kxml2.kdom.Element;

import net.ayaslive.miniim.common.Account;
import net.ayaslive.miniim.common.Contact;
import net.ayaslive.miniim.ui.core.Window;
import net.ayaslive.miniim.ui.markup.extended.DefaultComplexListener;
import net.ayaslive.miniim.ui.markup.extended.MarkupList;
import net.ayaslive.miniim.utils.io.resource.ResourceConnector;
import net.ayaslive.miniim.utils.io.resource.ResourceFetcher;
import net.ayaslive.miniim.utils.io.resource.ResourceIdentifier;
import net.ayaslive.miniim.utils.simplelogger.SimpleLogListener;
import net.ayaslive.miniim.utils.simplelogger.SimpleLogger;


final class MiniWebControler extends DefaultComplexListener implements ResourceConnector
//#ifdef LOG_VIEWER
,SimpleLogListener,Runnable
//#endif
{

	private static ResourceIdentifier HOME = new ResourceIdentifier(null, "res://main.xml");
	static MiniWebControler singleton = null;
	private Hashtable persistentImages = new Hashtable();
	
	MiniWebControler() {
		super(new MarkupList("Browser", ""));
		ResourceFetcher.getInstance().init(this);
		ResourceFetcher.getInstance().registerConnector("app", this);
		pushWrapper(MiniWebControler.HOME, false);
		
		// load settings..
		readSettings();
		
		// set presence
		Account.setPresenceToAll(status, message);
		
		// try to connect if autoconnect is activated
		Enumeration en = Account.getAll();
		while(en.hasMoreElements()) {
			Account acc = (Account)en.nextElement();
			// see if we need to do auto login
			String autoLogin = acc.get("ui.autologin");
			if(autoLogin == null || autoLogin.equals(String.valueOf(true))) {
				acc.connect();
			}
		}
	}
	
	static void doDestroy() {
		singleton.saveSettings();
	}
	
	public boolean pushWrapper(ResourceIdentifier id, boolean back) {
		// do magic: application switch !
		String url = id.getURL();
		if(url.equals("app://contacts")) {
			// BuddyList.singleton.show();
			// return super.pushWrapper(id, back);
		} else if(url.equals("app://accounts")) {
			AccountListDialog.singleton.show();
		} else if(url.equals("app://Settings.OK")) {
			
			// get the status value
			String label = id.get("Status");
			if(label.equals("Available")) {
				defaultStatus = Contact.AVAILABLE;
			} else if(label.equals("Away")) {
				defaultStatus = Contact.AWAY;
			} else if(label.equals("Dnd")) {
				defaultStatus = Contact.DND;
			} else {
				defaultStatus = Contact.UNAVAILABLE;
			}
			defaultMessage = id.get("Message");
			Account.setPresenceToAll(status, message);
			
			// getback
			return super.pushWrapper(HOME, true);
		} else if(url.equals("app://AccountDialog.OK")) {
			synchronized (this) {
				Account acc = AccountListDialog.singleton.getAccount();
				acc.setName(id.get("Name"));
				acc.setPassword(id.get("Password"));
				// acc.setProtocol(id.get("Protocol"));
				acc.setProtocol(Account.PROTOCOLS[0]);
				acc.setDomain(id.get("Domain"));
				acc.setHost(id.get("Server"));
				int port = -1;
				try {
					port = Integer.parseInt(id.get("port"));
				} catch(NumberFormatException e) {
				}
				acc.setPort(port);
				
				int autoRetry = -1;
				try {
					autoRetry = Integer.parseInt(id.get("Auto Retry"));
				} catch(NumberFormatException e) {
				}
				acc.setAutoRetry(autoRetry);
				acc.put("connector.url", id.get("Url"));
				
				String autoLogin = id.get("Auto Login");
				acc.put("ui.autologin", autoLogin);
				
				// auto connect ..
				if(autoLogin != null && autoLogin.equals(String.valueOf(true))) {
					acc.connect();
				}
			}

			// getback
			super.pushWrapper(HOME, true);
			AccountListDialog.singleton.show();
		} else if(url.equals("app://AccountDialog.Cancel")) {
			// getback
			super.pushWrapper(HOME, true);
			AccountListDialog.singleton.show();
		} else if(url.equals("app://home")) {
			return super.pushWrapper(HOME, true);
		} else if(url.equals("app://quit")) {
			App.destroy();
			// #ifdef LOG_VIEWER
		} else if(url.equals("app://LogViewer.OK")) {
			
			// set debug level
			String level = id.get("Level");
			int intLevel = SimpleLogger.ERROR;
			if(level == null || level.equals("Error")) {
				intLevel = SimpleLogger.ERROR;
			} else if(level.equals("Fatal")) {
				intLevel = SimpleLogger.FATAL;
			} else if(level.equals("Warn")) {
				intLevel = SimpleLogger.WARN;
			} else if(level.equals("Info")) {
				intLevel = SimpleLogger.INFO;
			} else if(level.equals("Debug")) {
				intLevel = SimpleLogger.DEBUG;
			}
			SimpleLogger.getRootLogger().setLevel(intLevel);
			
			// set auto update
			String autoUpdate = id.get("Auto Update");
			if(autoUpdate != null && autoUpdate.equals(String.valueOf(true))) {
				SimpleLogger.getRootLogger().start();
			} else {
				SimpleLogger.getRootLogger().end();
			}
			
			// getback
			super.pushWrapper(HOME, true);
		} else if(url.equals("app://LogViewer.Clear")) {
			SimpleLogger.getRootLogger().reset();
		} else if(url.equals("app://LogViewer.Send")) {
			new Thread(this).start();
			// #endif
		} else {
			return super.pushWrapper(id, back);
		}
		return false;
	}
	
	public void handleContent(String url, Object obj) {
		// inject values to local forms ..
		if(url.equals("res://im/Settings.xml")) {
			Document doc = (Document)obj;
			Element list = doc.getElement(null, "x").getElement(null, "ls");

			// reset default values
			status = defaultStatus;
			message = defaultMessage;
			
			// get the status element
			setOptionInSelectionBox(list.getElement(null, "s"), presenceInt2LabelIndex(this.status), false);
			
			// get the message element
			setTextInTextInput(list.getElement(null, "t"), this.message);
		} else if(url.equals("res://im/AccountDialog.xml")) {
			Document doc = (Document)obj;
			Element list = doc.getElement(null, "x").getElement(null, "ls");
			
			Account acc = AccountListDialog.singleton.getAccount();
			String name = acc.getName();
			
			final int count = list.getChildCount();
			
			for(int i=0; i < count; i++) {
				if(i == 0) {
					setTextInTextInput(list.getElement(i), name);
				} else if(i == 1) {
					setTextInTextInput(list.getElement(i), acc.getPassword());
				} else if(i == 2) {
					setOptionInSelectionBox(list.getElement(i), 0, false); // \xxx we have only one protocol implemented now :(
				} else if(i == 3) {
					setTextInTextInput(list.getElement(i), acc.getDomain());
				} else if(i == 4) {
					setTextInTextInput(list.getElement(i), acc.getServer());
				} else if(i == 5) {
					setTextInTextInput(list.getElement(i), String.valueOf(acc.getPort()));
				} else if(i == 6) {
					setTextInTextInput(list.getElement(i), acc.get("connector.url"));
				} else if(i == 7) {
					
					boolean autoLogin = true;
					try {
						String login = acc.get("ui.autologin");
						if(login != null) {
							// By default it is true
							autoLogin = !login.equals("false");
						}
					} catch(Exception e) {
						e.printStackTrace();
					}
					if(autoLogin) {
						list.getElement(i).setAttribute(null, "c", "y");
					} else {
						list.getElement(i).setAttribute(null, "c", null);
					}
				} else if(i == 8) {	
					setTextInTextInput(list.getElement(i), String.valueOf(acc.getAutoRetry()));
				}
			}
			
			// set an account title according to the user name
			list.setAttribute(null, "t", "Account - " + name);
		} else if(url.equals("res://im/LogViewer.xml")) {
			// #ifdef LOG_VIEWER
			Document doc = (Document)obj;
			Element list = doc.getElement(null, "x").getElement(null, "ls");
			
			final int count = list.getChildCount();
			
			for(int i=0; i < count; i++) {
				if(i == 0) {
					list.getElement(i).setAttribute(null, "c", SimpleLogger.getRootLogger().isRunning()? "y" : null);
				} else if(i == 1) {
					setOptionInSelectionBox(list.getElement(i), SimpleLogger.getRootLogger().getLevel(), false);
				} else if(i == 2) {
					setTextInTextInput(list.getElement(i), SimpleLogger.LEVELS[lastLogType]);
				} else if(i == 3) {
					setTextInTextInput(list.getElement(i), lastLogClassDesc);
				} else if(i == 4) {
					setTextInTextInput(list.getElement(i), lastLogStr);
				} else if(i == 5) {
					setTextInTextInput(list.getElement(i), lastLogExpClass);
				} else if(i == 6) {
					setTextInTextInput(list.getElement(i), lastLogTrace);
				}
			}
			// #else
//@			return;
			// #endif
		}
		
		if(url.startsWith("res://") && url.endsWith(".png")) {
			// store the images in persistent storage
			persistentImages.put(url, obj);
		}
		
		super.handleContent(url, obj);
	}
	
	public Image getImage(String src) {
		// find the image in the persistent place
		Image ret = (Image)persistentImages.get(ResourceIdentifier.getRealIdentifier(base, src));
		if(ret == null) {
			return super.getImage(src);
		}
		return ret;
	}
	
	public ContentConnection open(String url) throws IOException {
		// \todo load xmpp streamed ui data
		throw new IOException("Not implemented !");
	}
	
	/** Settings page */
	/*@{*/
	// default
	private String defaultStatus = Contact.AVAILABLE;
	private String defaultMessage = "Available";
	// values we are editing ..
	private String status = defaultStatus;
	private String message = defaultMessage;

	private final static int presenceInt2LabelIndex(String statusCode) {
		return statusCode.equals(Contact.AVAILABLE)?0:statusCode.equals(Contact.AWAY)?1:statusCode.equals(Contact.DND)?2:3;
	}
	/*@}*/
	
	/** @name settings load and save */
	/*@{*/
	private void readSettings() {
		RecordStore settings = null;
		try {
			// recreate
			settings = RecordStore.openRecordStore("Settings", true, RecordStore.AUTHMODE_PRIVATE, false);
			RecordEnumeration set = settings.enumerateRecords(null, null, false);
			while(set.hasNextElement()) {
				ByteArrayInputStream bais = new ByteArrayInputStream(set.nextRecord());
	            DataInputStream dis = new DataInputStream(bais);
	            while(true) {
	            	String key = dis.readUTF();
	            	if(key == null) {
	            		break;
	            	} else if(key.equals("default.status")) {
	            		defaultStatus = dis.readUTF();
	            	} else if(key.equals("default.message")) {
	            		defaultMessage = dis.readUTF();
	            	} else {
	            		// skip this value
	            		SimpleLogger.error(this, "readSettings()\t\tUnknown: " + key);
	            		break;
	            	}
	            }
			}
		} catch (RecordStoreFullException e) {
			SimpleLogger.error(this, "readSettings()", e);
		} catch (RecordStoreNotFoundException e) {
			SimpleLogger.error(this, "readSettings()", e);
		} catch (RecordStoreException e) {
			SimpleLogger.error(this, "readSettings()", e);
		} catch (EOFException e) {
			SimpleLogger.debug(this, "readSettings() read ..");
		} catch (IOException e) {
			SimpleLogger.error(this, "readSettings()", e);
		} finally {
			if(settings != null) {
				try {
					settings.closeRecordStore();
				} catch (RecordStoreNotOpenException e) {
					e.printStackTrace();
				} catch (RecordStoreException e) {
					e.printStackTrace();
				}
			}
		}
	}

	private void saveSettings() {
		RecordStore settings = null;
		try {
			// delete all existing records
			RecordStore.deleteRecordStore("Settings");
		} catch(Exception e) {
			e.printStackTrace();
			SimpleLogger.error(null, "destroy()", e);
		}
		try {
			// recreate
			byte[] bytes;
			settings = RecordStore.openRecordStore("Settings", true, RecordStore.AUTHMODE_PRIVATE, true);
			ByteArrayOutputStream os = new ByteArrayOutputStream();
            DataOutputStream dos = new DataOutputStream(os);
            dos.writeUTF("default.status");
            dos.writeUTF(defaultStatus);
            dos.writeUTF("default.message");
            dos.writeUTF(defaultMessage);
            os.close();
            bytes = os.toByteArray();
			settings.addRecord(bytes, 0, bytes.length);
		} catch (RecordStoreFullException e) {
			SimpleLogger.error(null, "destroy()", e);
		} catch (RecordStoreNotFoundException e) {
			SimpleLogger.error(null, "destroy()", e);
		} catch (RecordStoreException e) {
			SimpleLogger.error(null, "destroy()", e);
		} catch (IOException e) {
			SimpleLogger.error(null, "destroy()", e);
		} finally {
			if(settings != null) {
				try {
					settings.closeRecordStore();
				} catch (RecordStoreNotOpenException e) {
					e.printStackTrace();
				} catch (RecordStoreException e) {
					e.printStackTrace();
				}
			}
		}
		
	}
	/*@}*/

	static void show() {
		singleton.ml.show();
	}

	public ContentConnection open(String url, Hashtable postData)
			throws IOException {
		// TODO Auto-generated method stub
		return null;
	}

	// #ifdef LOG_VIEWER
	// \todo add a button to say that it is working and add a review ..
	private int lastLogType = 0;
	private String lastLogClassDesc = null;
	private String lastLogStr = null;
	private String lastLogExpClass = null;
	private String lastLogTrace = null;
	
	/** SimpleLogListener implementation */
	/*@{*/
	public void trace(int type, String classDesc, int hashCode, int threadHashCode, String str, String exceptionClassDesc, String trace) {
		this.lastLogType = type;
		this.lastLogClassDesc = classDesc;
		this.lastLogStr = str;
		this.lastLogExpClass = exceptionClassDesc;
		this.lastLogTrace = trace;
		if(type == SimpleLogger.INFO) {
			Window.pushBalloon(str, null, hashCode(), 5000);
		} else if(type == SimpleLogger.ERROR) {
			Window.pushBalloon(str, null, hashCode(), 5000);
		}
		// todo set the values realtime !
		// see if we are showing logviewer
//		String url = getCurrentURL();
//		if(url != null && url.equals("res://im/LogViewer.xml")) {
//			ml.repaint();
//		}
	}
	
	public void changed() {
	}
	/*@}*/
	/** Implementation of Runnable */
	/*@{*/
	public void run() {
		SimpleLogger.getRootLogger().save();
	}
	/*@}*/
	// #endif
}
