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

package net.ayaslive.miniim.ui.markup.extended;

import java.io.IOException;
import java.io.InputStream;
import java.util.Hashtable;
import java.util.Vector;

import javax.microedition.lcdui.Image;
import javax.microedition.lcdui.TextField;

import net.ayaslive.miniim.ui.core.ActionListener;
import net.ayaslive.miniim.ui.core.Menu;
import net.ayaslive.miniim.ui.core.TextEdit;
import net.ayaslive.miniim.ui.core.TextEditListener;
import net.ayaslive.miniim.ui.core.Window;
import net.ayaslive.miniim.ui.markup.EventListener;
import net.ayaslive.miniim.ui.markup.MediaLoader;
import net.ayaslive.miniim.utils.io.resource.ResourceFetcher;
import net.ayaslive.miniim.utils.io.resource.ResourceIdentifier;
import net.ayaslive.miniim.utils.io.resource.ResourceListener;
import net.ayaslive.miniim.utils.simplelogger.SimpleLogger;

import org.kxml2.kdom.Document;
import org.kxml2.kdom.Element;
import org.kxml2.kdom.Node;


public class DefaultComplexListener implements EventListener, MediaLoader, ActionListener, ResourceListener, TextEditListener {

	// \xxx should we keep the images in the recordstore to save memory in heap ?
	private final Hashtable images = new Hashtable();
	public final static String MINI_WEB = "MiniWeb"; // should I keep this field public ?
	private final static String BACK = "Back";
	private final static String OK = "OK";
	
	private final static int END_INDEX = 3; // so you have 3 volatile menu commands
	private Document currentDoc = null;
	protected String base = null;
	private String currentUrl = null;
	private boolean goingBack = false; // a flag to indicate that we are going back
	protected boolean loadingPage = false; // a flag to indicate that we are busy loading a page
	
	protected MarkupList ml = null;
	protected final Vector stack = new Vector();
	
	public DefaultComplexListener(MarkupList ml) {
		this.ml = ml;
		ml.setActionListener(this);
		ml.setEventListener(this);
		ml.setMediaLoader(this);
		ml.setRightOption(BACK);
	}
	
	public void handleEvent(Element elem, int type) {
		SimpleLogger.debug(this, "handleEvent()\t\tLoading url: " + elem.getAttributeValue("href"));
		// add a baloon ..
		pushWrapper(elem.getAttributeValue("href"), false);
	}
	
	protected String getCurrentURL() {
		return currentUrl;
	}

	private Element showingSelectionBox = null;
	private int oldSelection = 0;
	public void performAction(String action) {
		// selection box mode ....
		if(showingSelectionBox != null) {
			
			// see if it supports multiple selection ..
			if(isPositiveAttribute(showingSelectionBox, "m")) {
				
				// so it is multiple selection box .. 
				
				// see if he is toggling ..
				if(action != OK) {
					
					// toggle selection
					setOptionInSelectionBox(showingSelectionBox, ml.getSelectedIndex(), true);
					return;
				}
				
				// otherwise he is going back ..
			} else {
			
				// set the selection index ..
				setOptionInSelectionBox(showingSelectionBox, ml.getSelectedIndex(), false);
			}

			// get back to the old page ..
			Element ls = currentDoc.getElement(null, "x").getElement(null, "ls");
			ml.setNode(ls, getDefaultSelectedItem(ls));
			
			// fix the title
			String title = ls.getAttributeValue("t");
			if(title == null) {
				title = MINI_WEB;
			}
			ml.setTitle(title);
			
			// fix the selected index
			ml.setSelectedIndex(oldSelection);
			
			// fix the menu
			ml.show();
			showingSelectionBox = null;
			return;
		}
		
		
		// Normal mode ..
		if(action == ml.defaultCommand) {
			SimpleLogger.debug(this, "item action !");
			Element item = ml.getSelectedItem();
			String ref = item.getAttributeValue("href");
			if(ref != null) {
				pushWrapper(ref, false);
				return;
			}
			
			// so it is not link see if it is text field ..
			final String tag = item.getName();
			if(tag.equals("t")) {
				
				// get the text
				String text = null;
				if(item.getChildCount() == 0 || (text = item.getText(0)) == null) {
					
					// do not allow null
					text = "";
				}
				
				// create renderer
				TextEdit.getInstance().showTextEdit(this, item.getAttributeValue("l"), text, 100, (isPositiveAttribute(item, "p"))? TextField.PASSWORD : TextField.ANY);
			} else if(tag.equals("s")) {
				
				// we render the selection box
				showingSelectionBox = item;
				
				// \todo override the title
				ml.setTitle(item.getAttributeValue("l"));
				ml.setNode(item, 0);
				
				// save the old selected index
				oldSelection = ml.getSelectedIndex();
				
				// set selected index
				final int count = showingSelectionBox.getChildCount();
				for(int i=0; i<count; i++) {
					Element elem = showingSelectionBox.getElement(i);
					// see if it is selected
					if(DefaultComplexListener.isPositiveAttribute(elem, "s")) {
						ml.setSelectedIndex(i);
					}
				}

				if(isPositiveAttribute(showingSelectionBox, "m")) {
					
					// override the menu , show OK
					Menu.getInstance().setMenu(OK, null);
				} else {
					
					// override the menu and show only go !
					Menu.getInstance().setMenu(null, null);
				}
				
				ml.repaint();
			} else if(tag.equals("ch")){

				// toggle checkbox state ..
				if(DefaultComplexListener.isPositiveAttribute(item, "c")) {
					item.setAttribute(null, "c", null);
				} else {
					item.setAttribute(null, "c", "y");
				}
				ml.repaint();
			} else if(tag.equals("r")) {
				
				if(DefaultComplexListener.isPositiveAttribute(item, "c")) {
					return;
				}
				
				// get the radio button name ..
				String name = item.getAttributeValue("n");
				if(name == null) {
					return;
				}
				
				// find other radio buttons with the same name ..
				Element ls = (Element)item.getParent();
				Element each = null;
				String eachName = null;
				int count = ls.getChildCount();
				
				// traverse all the items
				for(int i=0; i < count; i++) {
					
					each = (Element)ls.getChild(i);
					
					SimpleLogger.debug(this, "performAction()\t\tElement -> " + count + ":" + i + ":" + each.getName());
					// skip the current item ..
					if(each == item) {
						continue;
					}
					
					// we are only interested about the radio buttons
					if(!each.getName().equals("r")) {
						continue;
					}
					
					// mathch the group
					eachName = each.getAttributeValue("n");
					if(eachName == null || !name.equals(eachName)) {
						continue;
					}
					
					SimpleLogger.debug(this, "performAction()\t\tunchecking " + each.getAttributeValue("l"));
					// uncheck
					each.setAttribute(null, "c", null);
				}
				
				item.setAttribute(null, "c", "y");
				
				ml.repaint();
			}
		} else if(action == BACK) {
			String last = (String)stack.lastElement();
			pushWrapper(last, true);
		} else {
			submitForm(currentDoc, action);
		}
	}
	
	public final synchronized void submitForm(Document doc, String action) {
		// It is dynamic action
		if(doc == null) {
			return;
		}
		try {
			Element x = doc.getRootElement();
			int count = x.getChildCount();
			for(int i=0; i<count; i++) {
				
				if(x.getType(i) != Node.ELEMENT) {
					continue;
				}
				// find a command
				Element cmd = x.getElement(i);
				String name = cmd.getName();
				if(!name.equals("c")) {
					continue;
				}
				
				// find the command match our action
				String label = cmd.getText(0);
				if(label == null) {
					continue;
				}
				if(!label.equals(action)) {
					continue;
				}
				
				// find the reference of the command
				String ref = cmd.getAttributeValue("href");
				if(ref == null) {
					return;
				}
				SimpleLogger.debug(this, "submitForm() => matched: " + ref);
				
				ResourceIdentifier id = new ResourceIdentifier(base, ref);
				
				// put the data fields
				Element list = x.getElement(null, "ls");
				int elemCount = list.getChildCount();

				// XXX We are assuming that the radio buttons with same name will be adjacent .. so we are using a radioCheckedFlag .. It will not work on random radio button position of different radio button group ..
				boolean radioCheckedFlag = true;
				for(int j=0; j < elemCount; j++) {
					Element elem = (Element)list.getChild(j);
					final String tag = elem.getName();
					final String key = elem.getAttributeValue("n");
					SimpleLogger.debug(this, "submitForm() => working on: " + elem.getName());
					if(key == null) {
						continue;
					}
					
					if(tag.equals("ch")) {
						
						SimpleLogger.debug(this, "submitForm() => key:" + key);
						
						if(DefaultComplexListener.isPositiveAttribute(elem, "c")) {
							SimpleLogger.debug(this, "submitForm() => value:checked");
							id.put(key, String.valueOf(true));
						} else {
							SimpleLogger.debug(this, "submitForm() => value:not checked");
						}
						continue;
					}
					
					if(tag.equals("r")) {
						
						// see if the key is already set ..
						if(id.get(key) != null) {
							continue;
						}
						
						String value = elem.getAttributeValue("v");
						if(value != null && (radioCheckedFlag = DefaultComplexListener.isPositiveAttribute(elem, "c"))) {
							SimpleLogger.debug(this, "submitForm() => key:" + key);
							SimpleLogger.debug(this, "submitForm() => value:" + value);
							id.put(key, value);
						}
						continue;
					}
					
					if(tag.equals("s")) {
						
						SimpleLogger.debug(this, "submitForm() => key:" + key);
						// search selected item ..
						final int opCount = elem.getChildCount();
						boolean isMultipleSelection = isPositiveAttribute(elem, "m");
						String value = "";
						for(int k=0, p=0; k<opCount; k++) {
							Element op = elem.getElement(k);
							
							// see if it is selected
							if(DefaultComplexListener.isPositiveAttribute(op, "s")) {
						
								// found selected item
								value = op.getText(0);
								SimpleLogger.debug(this, "submitForm() => value:" + value);
								if(isMultipleSelection) {
									
									// \xxx String concatanation optimization needed here ..
									id.put(key + '[' + p + ']', value);
								} else {
									id.put(key, value);
									break;
								}
								p++;
							}
						}
						
						// see if this field is enforced
						if(value.length() == 0 && DefaultComplexListener.isPositiveAttribute(elem, "r")) {
							// so this field is mandatory .. 
							Window.pushBalloon(key + " cannot be empty", null);
							return;
						}
						
						continue;
					}
					
					if(tag.equals("t")) {
						
						SimpleLogger.debug(this, "submitForm() => key:" + key);
						
						// get the text
						String value = null;
						if(elem.getChildCount() == 0 || (value = elem.getText(0)) == null) {
							
							// see if this field is enforced
							if(DefaultComplexListener.isPositiveAttribute(elem, "r")) {
								// so this field is mandatory .. 
								Window.pushBalloon(key + " cannot be empty", null);
								return;
							}
							
							// do not allow null
							value = "";
						} else {
							
							// get rid of spaces
							value = value.trim();
						}
						
						SimpleLogger.debug(this, "submitForm() => value:" + value);
						
						id.put(key, value);
						continue;
					}
				}
				
				// see if there is a checked value for the radio button groups ..
				if(!radioCheckedFlag) {
					Window.pushBalloon("Please check the radio buttons ", null);
					return;
				}
				
				// see if the form data should be preserved ..
				if(isPositiveAttribute(x, "cf")) {
					
					try {
						// delete the old
						Document.delete(MINI_WEB, null, doc.getRecordId(), false);
						
						// save the updated one
						doc.store(MINI_WEB, doc.getUrl(), false, 1000000); // FIXME the timeout is not set correctly
					} catch(Exception e) {
						SimpleLogger.error(this, "submitForm()\t\tCould not backup form.", e);
					}
				}
				pushWrapper(id, false);
				return;
			}
		} catch(RuntimeException e) {
			SimpleLogger.debug(this, "submitForm() => no commands", e);
		}
	}

	public void handleContent(String url, Object obj) {
		// \todo set menu command ..
		if(obj instanceof InputStream) {
			Window.pushBalloon(null, null, hashCode(), 0);
			try {
				SimpleLogger.debug(this, "handleContent()\t\t[+]Image -> " + url);
				images.put(url, Image.createImage((InputStream)obj));
			} catch (IOException e) {
				e.printStackTrace();
			}
			ml.repaint();
			return;
		}
		Document doc = (Document)obj;
		try {
			Element x = doc.getElement(null, "x");
			Element list = x.getElement(null, "ls");
			Window.pushBalloon(null, null, hashCode(), 0);
			synchronized (this) {
				if(currentUrl != null && !goingBack) {
						if(currentUrl == null || !currentUrl.equals(url)) {
							SimpleLogger.debug(this, "handleContent()\t\t" + currentUrl + "\t\t[+]");
							stack.addElement(currentUrl);
						} else {
							SimpleLogger.debug(this, "handleContent()\t\t" + currentUrl + "\t\t[*]");
						}
				} else if(goingBack){
					SimpleLogger.debug(this, "handleContent()\t\t" + stack.lastElement() + "\t\t[-]");
					stack.removeElement(stack.lastElement());
				}
				currentUrl = url;
				
				// save the current doc and base
				currentDoc = doc;
				int last = url.lastIndexOf('/');
				if(last == -1) {
					base = url + "/";
				} else {
					base = url.substring(0, last+1);
				}
			}
			resetMenu();
			try {
				int count = x.getChildCount();
				for(int i=0,j=0; i<count; i++) {
					if(x.getType(i) != Node.ELEMENT) {
						continue;
					}
					Element cmd = x.getElement(i);
					String name = cmd.getName();
					if(!name.equals("c")) {
						continue;
					}
					String label = cmd.getText(0);
					if(label == null) {
						continue;
					}
					// found a new command ..
					if(DefaultComplexListener.isPositiveAttribute(cmd, "r")) {
						ml.setRightOption(label);
					} else {
						ml.setLeftOption(j++, label);
					}
				}
				
				String tmp = x.getAttributeValue("c");
				long cacheTimeout = (tmp != null)?(Integer.parseInt(tmp)*1000):-1;
				// See if we can cache it
				if(cacheTimeout != -1) {
					
					// see if it already cached
					if(!Document.exists(MINI_WEB, url)) {

						SimpleLogger.debug(this, "handleContent()\t\t[######] <<  " + url);
						// cache it
						doc.store(MINI_WEB, url, false, cacheTimeout);
					}
					
				}
			} catch(Exception e) {
				SimpleLogger.debug(this, "handleContent() => no commands");
			}
			String title = list.getAttributeValue("t");
			if(title == null) {
				title = MINI_WEB;
			}
			ml.setTitle(title);
			ml.setNode(list, getDefaultSelectedItem(list));
		} catch(RuntimeException e) {
			SimpleLogger.error(this, "handleContent()", e);
		} finally {
			clearFlags();
		}
	}
	
	protected void resetMenu() {
		if(!stack.isEmpty()) {
			ml.setRightOption(BACK);
		} else {
			ml.setRightOption(null);
		}
		// clear left menu
		for(int i=0; i < END_INDEX; i++) {
			ml.setLeftOption(i, null);
		}
	}
	
	protected void clearHistory() {
		stack.removeAllElements();
	}

	public void handleError(String url, int code, String reason) {
		clearFlags();
		SimpleLogger.error(this, "handleError()\t\t" + reason);
		
		// FIXME wrong idea, we are putting a placeholder of image even when document loading failed ..
		try {
			images.put(url, Image.createImage("/ui/error.png"));
		} catch (IOException e) {
			SimpleLogger.debug(this, "handleError()\t\tCould not load placeholder image, it may happen that the application gets into a loop loading image ..");
		}
		Window.pushBalloon("Error ..", null, hashCode(), 2000);
	}
	
	public void handleProgress(String url, int step, String hint) {
		Window.pushBalloon(hint, null, hashCode(), 100000000);
	}
	
	
	public synchronized Image getImage(String src) {
		SimpleLogger.debug(this, "getImage()\t\t[*]Image -> " + ResourceIdentifier.getRealIdentifier(base, src));
		// \todo do some cache size checkup ..
		Image ret = (Image)images.get(ResourceIdentifier.getRealIdentifier(base, src));
		if(ret == null) {
			ResourceFetcher.getInstance().push(new ResourceIdentifier(base, src));
		}
		return ret;
	}
	
	protected final synchronized boolean pushWrapper(String url, boolean back) {
		return pushWrapper(new ResourceIdentifier(base, url), back);
	}
	
	protected synchronized boolean pushWrapper(ResourceIdentifier id, boolean back) {
		if(loadingPage) { // check if we are on action ..
			SimpleLogger.debug(this, "We are still loading a page, we cannot perform other actions ..");
			return false;
		}
		
		Window.pushBalloon("Loading ..", null, hashCode(), 100000000);
		images.clear();
		loadingPage = true;
		goingBack = back;
		
		try {
			// #debug error
			Document.dump(MINI_WEB);
			// see if it is in the cache
			Document doc = new Document();
			
			// search the url and load
			if(doc.load(MINI_WEB, id.getURL(), false)) {
				
				SimpleLogger.debug(this, "pushWrapper()\t\t[######] >> " + id.getURL());
				// found the url, so render it
				handleContent(id.getURL(), doc);
				return true;
			}
		} catch(Exception e) {
			e.printStackTrace();
		}
		
		ResourceFetcher.getInstance().push(id);
		return true;
	}

	public void textEdited(String newText) {
		Element elem = ml.getSelectedItem();
		setTextInTextInput(elem, newText);
		ml.repaint();
	}
	
	protected synchronized void clearFlags() {
		loadingPage = false;
		goingBack = false;
		Window.pushBalloon(null, null, hashCode(), 0);
	}
	
	public final static void setOptionInSelectionBox(Element selectionbox, int index, boolean toggle) {
		final int count = selectionbox.getChildCount();
		final String positive = "y";
		String val;
		for(int i=0; i<count; i++) {
			if(toggle && i != index) {
				
				// this is not the element of our interest
				continue;
			}
			val = null;
			Element elem = selectionbox.getElement(i);
			if(toggle) {
				
				// so see if it is selected ..
				if(DefaultComplexListener.isPositiveAttribute(elem, "s")) {
					
					// uncheck
					val = null;
				} else {
					
					// check
					val = positive;
				}
			} else {
				if(i == index) {
					
					// check
					val = positive;
				}
			}
			elem.setAttribute(null, "s", val);
		}
	}
	
	public final static void setTextInTextInput(Element textInput, String newText) {
		final int count = textInput.getChildCount();
		for(int i=0; i < count; i++) {
			textInput.removeChild(0);
		}
		
		if(newText != null && (newText = newText.trim()).length() != 0) {
			
			// so we have solid text
			textInput.addChild(Node.TEXT, newText);
		}
	}
	
	public final static boolean isPositiveAttribute(Element elem, String field) {
		String tmp = elem.getAttributeValue(field);
		return tmp != null && tmp.equals("y");
	}
	
	public final static void setLink(Element elem, String link, String label) {
		elem.clear();
		elem.setAttribute("href", link);
		SimpleLogger.debug(null, "DefaultComplexListener::setLink()\t\t" + elem + "-" + link + "-" + label);
		elem.addChild(0, Node.TEXT, label);
	}
	
	public final static int getDefaultSelectedItem(Element elem) {
		int count = elem.getChildCount();
		for(int i=0; i < count; i++) {
			if(isPositiveAttribute((Element)elem.getChild(i), "s")) {
				return i;
			}
		}
		return 0;
	}
}
