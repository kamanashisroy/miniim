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

import java.util.Enumeration;

import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Image;

import net.ayaslive.miniim.ui.core.List;
import net.ayaslive.miniim.ui.core.Menu;
import net.ayaslive.miniim.ui.core.Window;
import net.ayaslive.miniim.ui.core.list.ListItem;
import net.ayaslive.miniim.ui.core.list.ListItemFactory;
import net.ayaslive.miniim.ui.markup.EventListener;
import net.ayaslive.miniim.ui.markup.MarkupItem;
import net.ayaslive.miniim.ui.markup.MediaLoader;
import net.ayaslive.miniim.utils.simplelogger.SimpleLogger;

import org.kxml2.kdom.Element;
import org.kxml2.kdom.Node;

public class MarkupList extends List implements Enumeration {
	private Node node = null;
	private EventListener el = null;
	private MediaLoader ml = null;
	protected String defaultCommand = null;
	private String rightOption = null;
	private final String[] leftMenuOptions = {null, null, null, null, null, null, null, null};
	private final static StringBuffer buff = new StringBuffer();
	private int lastRow = 0;
	private int lastCol = 0;
	private long lastEvent = 0;
	private boolean keyRepeated = false;
	private boolean searching = false;
	private boolean isMultipleSelection = false;
	private char[][] keys = {
			{'0', ' '},
			{'1', '.', ','},
			{'2', 'a', 'b', 'c', 'A', 'B', 'C'},
			{'3', 'd', 'e', 'f', 'D', 'E', 'F'},
			{'4', 'g', 'h', 'i', 'G', 'H', 'I'},
			{'5', 'j', 'k', 'l', 'J', 'K', 'L'},
			{'6', 'm', 'n', 'o', 'M', 'N', 'O'},
			{'7', 'p', 'q', 'r', 's', 'P', 'Q', 'R', 'S'},
			{'8', 't', 'u', 'v', 'T', 'U', 'V'},
			{'9', 'w', 'x', 'y', 'z', 'W', 'X', 'Y', 'Z'},
	};
	
	public MarkupList(String title, String defaultCommand) {
		super("Products", defaultCommand);
		this.defaultCommand = defaultCommand;
	}
	
	public void setEventListener(EventListener el) {
		this.el = el;
	}
	
	public void setMediaLoader(MediaLoader ml) {
		this.ml = ml;
	}

	public Element getSelectedItem() {
		return node.getElement(super.getSelectedIndex());
	}
	
	protected void setSelectedIndex(int index) {
		super.setSelectedIndex(index);
	}
	
	protected int getSelectedIndex() {
		return super.getSelectedIndex();
	}
	
	public void setNode(Node node, int selectedIndex) {
		this.node = node;
		SimpleLogger.debug(this, "setNode()\t\t" + node);
		searching = false;
		continuousScrolling = true;
		
		super.setSelectedIndex(selectedIndex);
		
		// see if the node is selection box ..
		if(node instanceof Element) {
			Element elem = (Element)node;
			if(elem.getName().equals("s")) {
				// see if it has multiple choice ..
				isMultipleSelection = DefaultComplexListener.isPositiveAttribute(elem, "m");
			}
		}
		this.repaint();
	}
	
	public void show() {
		searching = false;
		super.show(rightOption, leftMenuOptions);
	}
	
	public final void setRightOption(String rightOption) {
		this.rightOption = rightOption;
		// \xxx no locking :(
		if(isShowing()) {
			Menu.getInstance().setRightOption(rightOption);
		}
	}
	
	public final void setLeftOption(int pos, String command) {
		// \xxx no locking :(
		if(isShowing()) {
			Menu.getInstance().setLeftOption(pos, command);
		} else {
			leftMenuOptions[pos] = command;
		}
	}

	/// List implementation
	/*@{*/
	protected int getCount() {
		if(node == null) {
			return 0;
		}
		return node.getChildCount();
	}

	protected String getHint() {
		return null;
	}

	protected Enumeration getItems() {
		pos = 0;
		return this;
	}

	protected ListItem getListItem(Object node) {
		// get the element
		Element elem = (Element)node;
		final String name = elem.getName();
		final String label = elem.getAttributeValue("l");
		if(name == null || name.equals("m")) {
			return MarkupItem.getInstance(elem, ml, false, el);
		} else if(name.equals("l")) {
			String text = ".";
			if(elem.getChildCount() > 0) {
				text = elem.getText(0);
				if(text != null) {
					text = text.trim();
				} else {
					text = ".";
				}
			}
			
			// see if the label has any image
			Image img = null;
			String src = elem.getAttributeValue("src");
			if(src != null) {
				img = ml.getImage(src);
			}
			return ListItemFactory.createLabel(text, img, elem.getAttributeValue("href") != null, false);
		} else if(name.equals("t")) {
			
			// get current text
			String text = null;
			if(elem.getChildCount() == 0 || (text = elem.getText(0)) == null) {
				
				// get hint of this field
				text = elem.getAttributeValue("h");
				if(text == null) {
					text = "";
				}
			} else {
				
				// get rid of spaces
				text = text.trim();
			}
			
			// see if it is password field
			if(DefaultComplexListener.isPositiveAttribute(elem, "p")) {
				
				// in this case we hide the content of the password ..
				text = "**********************************".substring(0, text.length());
			}
			
			// do not scroll continuously when there is textfield
			continuousScrolling = false;
			return ListItemFactory.createTextInput(label, text, DefaultComplexListener.isPositiveAttribute(elem, "w"), true);
		} else if(name.equals("s")) {
			
			// get selected index
			StringBuffer buffer = new StringBuffer();
			boolean first = true;
			final int count = elem.getChildCount();
			for(int i=0; i<count; i++) {
				Element op = elem.getElement(i);
				
				// see if it is selected
				if(DefaultComplexListener.isPositiveAttribute(op, "s")) {
					String tmp = op.getText(0);
					if(tmp == null) {
						continue;
					}
					if(first) {
						first = false;
					} else {
						buffer.append(',');
					}
					buffer.append(tmp.trim());
				}
			}
			
			// do not scroll continuously when there is selection box
			continuousScrolling = false;
			return ListItemFactory.createSelectionBox(label, buffer.toString(), true);
		} else if(name.equals("r")) {
			// render radio button
			
			return ListItemFactory.createRadioButton(label, DefaultComplexListener.isPositiveAttribute(elem, "c"), true);
		} else if(name.equals("ch")){
			// so it is checkbox
			
			return ListItemFactory.createCheckBox(label, DefaultComplexListener.isPositiveAttribute(elem, "c"), true);
		} else if(name.equals("o")){
			// so it is selection option
			// get current text
			String text = null;
			if(elem.getChildCount() == 0 || (text = elem.getText(0)) == null) {
				
				// get hint of this field
				text = elem.getAttributeValue("h");
				if(text == null) {
					text = "";
				}
			} else {
				
				// get rid of spaces
				text = text.trim();
			}

			// see if it is multiple selection box ..
			if(isMultipleSelection) {
				
				// see if it is selected ..
				return ListItemFactory.createCheckBox(text, DefaultComplexListener.isPositiveAttribute(elem, "s"), true);
			} else {
				return ListItemFactory.createLabel(text, null, true, false);
			}
		} else {
			return MarkupItem.getInstance(elem, ml, false, el);
		}
	}
	
	public boolean handleElement(int keyCode, int gameAction) {
		int index = super.getSelectedIndex();
		
		// avoid invalid selected index
		if(index >= node.getChildCount()) {
			return false;
		}
		
		// allow markup traversing
		Element elem = (Element)node.getChild(index);
		ListItem item = getListItem(elem);
		if(item instanceof MarkupItem) {
			if(((MarkupItem)item).keyPressed(keyCode, gameAction)) {
				item.free();
				return true;
			}
		}
		item.free();
		
		// see if it is any text
		if(keyCode < Canvas.KEY_NUM0 || keyCode > Canvas.KEY_NUM9) {
			item.free();
			searching = false;
			return false;
		}
		long curTime = System.currentTimeMillis();
		if(curTime - lastEvent > 1000 || (keyCode - Canvas.KEY_NUM0) != lastRow) { // 1 second
			
			// new key
			lastRow = keyCode - Canvas.KEY_NUM0;
			lastCol = 0;
			keyRepeated = false;
		} else {
			
			// updated key
			if(++lastCol > keys[lastRow].length) {
				lastCol = 0;
			}
			keyRepeated = true;
		}
		lastEvent = curTime;
		
		if(item instanceof ListItemFactory) {
			ListItemFactory li = (ListItemFactory)item;
			
			if(li.getType() == ListItemFactory.TEXT_INPUT) {
				
				// allow inline text editing
				doEdit(elem);
				item.free();
				return true;
			}
		}

		// allow searching
		doSearch();
		item.free();
		return true;
	}
	/*@}*/
	
	/** Searching */
	private void doSearch() {
		int length = 0;
		if(!searching) {
			buff.setLength(0);
			length = 0;
		} else {
			length = buff.length();
		}
		searching = true;
		if(keyRepeated) {
			
			// toggle the last character
			buff.setCharAt(length - 1, keys[lastRow][lastCol]);
		} else {
			
			// do not allow search prefix greater than 4 character
			if(length < 4) {
				buff.append(keys[lastRow][lastCol]);
			}
		}
		String prefix = buff.toString();
		Window.pushBalloon(prefix, null, hashCode(), 1000);
		boolean found = false; // found flag
		final int count = node.getChildCount();
		// traverse the child
		for(int i = 0; i < count && !found; i++) {
			Element elem = (Element)node.getChild(i);
			final int size = elem.getChildCount();
			for(int j = 0; j < size; j++) {
				if(elem.getType(j) == Node.TEXT) {
					if(elem.getText(j).startsWith(prefix)) {
						found = true;
						setSelectedIndex(i);
					}
					break;
				}
			}
		}
	}
	
	private void doEdit(Element elem) {
		String oldText = null;
		final int count = elem.getChildCount();
		for(int i=0; i < count; i++) {
			oldText = elem.getText(0);
			elem.removeChild(0);
		}
		buff.setLength(0);
		if(oldText != null) {
			buff.append(oldText);
		}
		
		if(keyRepeated) {
			buff.setCharAt(buff.length() - 1, keys[lastRow][lastCol]);
		} else {
			buff.append(keys[lastRow][lastCol]);
		}
		
		// so we have solid text
		elem.addChild(Node.TEXT, buff.toString());
		buff.setLength(0);
	}

	/** Enumeration implementation */
	/*@{*/
	private int pos = 0;
	public boolean hasMoreElements() {
		if(node == null) {
			return false;
		}
		return pos < node.getChildCount();
	}

	public Object nextElement() {
		return node.getChild(pos++);
	}
	/*@}*/
}
