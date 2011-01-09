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

//public final class MarkupItem implements ListItem {
struct markup_item {

	struct list_item list_item;
	struct element root; // Document root
	/**
	 * y-coordinate position of the image
	 */
	int xPos; // = 0
	int yPos;// = 0;

	Graphics g;// = null;

	int lineHeight; // = 0;
	int width;
	int minLineHeight;// = Font.getFont(Font.FACE_SYSTEM, Font.STYLE_PLAIN,Font.SIZE_SMALL).getHeight()+PADDING;
	boolean selected;// = false;
	media_loader loader;// = null;
	markup_item single_instance;// = null;

	int (*initialize)(void*data);
	void (*render_node)(struct font*font, struct element*elem);
	void (*render_text)(struct font*font, const char*text);
	void (*render_image)(struct element*elem);
	void (*break_line)();
	void (*clear_line)();
	void (*clear_line)(int y, int height);
	void (*update_height)(struct font*font);
	void (*update_height)(int newHeight);

	/** List item implementation .. */
	/*@{*/
	int (*paint)(Graphics g, int x, int y, int width, boolean selected);
	/*@}*/

	void (*free)();
	
	/// traverser
	/*@{*/
	private final Vector focusableElem = new Vector();
	private Element focusedElem = null;
	private EventListener listener = null;
	void set_listener(struct event_listener listener);
	
	private void freeTraverser() {
		focusedElem = null;
		focusableElem.removeAllElements();
		listener = null;
	}
	/** Key press listener to traverse the browsable elements */
	public boolean keyPressed(int keyCode, int gameAction) {
		searchFocusableElements(root);
		final int size = focusableElem.size();
		if(size == 0) {
			return false;
		}
		int index = 0;
		if(focusedElem != null) {
			index = focusableElem.indexOf(focusedElem);
		}
		
        if (gameAction == GameCanvas.UP) {
        	if(focusedElem == null) {
        		// focus the last element
        		setFocus((Element)focusableElem.lastElement());
        		return true;
        	}
        	
        	if(index == 0) {
        		// already focused the topmost element
        		return false;
        	}
        	
        	// decrease index
        	setFocus((Element)focusableElem.elementAt(index - 1));
        	return true;
        } else if (gameAction == GameCanvas.LEFT) {
        	if(focusedElem == null) {
        		// select the last node
        		setFocus((Element)focusableElem.lastElement());
        		return true;
        	}
        	if(index == 0) {
        		// already the first node selected
        		return false;
        	}
    		// focus the first element
        	setFocus((Element)focusableElem.firstElement());
    		return true;
        } else if (gameAction == GameCanvas.DOWN) {
        	
        	if(focusedElem == null) {
        		// go to the topmost element
        		setFocus((Element)focusableElem.firstElement());
        		return true;
        	}
        	
        	if(index == size - 1) {
        		// already selected the last element
        		return false;
        	}
        	
        	// increase index
        	setFocus((Element)focusableElem.elementAt(index + 1));
        	return true;
        } else if(gameAction == GameCanvas.RIGHT) {
        	if(focusedElem == null) {
        		setFocus((Element)focusableElem.firstElement());
        		return true;
        	}
        	if(index == size -1) {
        		// already the last node selected
        		return false;
        	}
    		// focus the last element
        	setFocus((Element)focusableElem.lastElement());
    		return true;
        } else if(gameAction == GameCanvas.FIRE) {
        	if(focusedElem == null || listener == null) {
        		return false;
        	}
        	listener.handleEvent(focusedElem, EventListener.CLICK);
        	return true;
        }
        return false;
	}
	
	private void searchFocusableElements(Element elem) {
		final String name = elem.getName();
		if(name.equals("a") || name.equals("input")) {
			focusableElem.addElement(elem);
			if(isFocused(elem)) {
				focusedElem = elem;
			}
			return;
		}
		final int count = elem.getChildCount();
		for(int i = 0; i < count; i++) {
			if(elem.getType(i) == Node.ELEMENT) {
				searchFocusableElements(elem.getElement(i));
			}
		}
	}
	
	private void setFocus(Element elem) {
		if(focusedElem != null) {
			focusedElem.setAttribute(null, "focused", null);
		}
		// SimpleLogger.debug(this, "setFocus()\t\t" + elem);
		elem.setAttribute(null, "focused", "yes");
	}
	
	
	private static boolean isFocused(Element elem) {
		final String focused = elem.getAttributeValue(null, "focused");
		if(focused != null && focused.equals("yes")) {
			return true;
		}
		return false;
	}
	
	private static boolean isActive(Element elem) {
		final String active = elem.getAttributeValue(null, "active");
		if(active != null && active.equals("yes")) {
			return true;
		}
		return false;
	}
	/*@}*/
}

struct markup_item*get_instance(struct element root, struct media_loader loader, boolean selectable, struct event_listener el);
