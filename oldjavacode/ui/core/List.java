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

// -- #define net.ayaslive.miniim.ui.core.list.draw_menu_at_last
// -- #define net.ayaslive.miniim.ui.core.rounded_corner_radius

package net.ayaslive.miniim.ui.core;

import java.util.Enumeration;

import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.game.GameCanvas;

import net.ayaslive.miniim.ui.core.list.ListItem;


/** \todo support object item showing, truncated text showing .. */
public abstract class List extends Window {
	/*! \todo show arrow signs(left and right arrow) to indicate that the text is truncated */
	protected static final int HMARGIN = 3;
	protected static final int VMARGIN = 2;
	
	protected boolean continuousScrolling = false;
	
	private int vpos = 0; /* Index of the showing Item */
	private int selectedIndex = 0;
	
	/** Styling */
	//#ifndef net.ayaslive.miniim.ui.core.list.bg
	
	// define style
	//#define net.ayaslive.miniim.ui.core.list.bg='0xFFFFFF'
	//#define net.ayaslive.miniim.ui.core.list.leftMargin=20
	//#define net.ayaslive.miniim.ui.core.list.topMargin=20
	//#define net.ayaslive.miniim.ui.core.list.bottomMargin=20
	//#define net.ayaslive.miniim.ui.core.list.rightMargin=20
	//#define net.ayaslive.miniim.ui.core.list.indicator='0x006699'
	
	//#endif
	
	//#expand private static int leftMargin = %net.ayaslive.miniim.ui.core.list.leftMargin%;
	private static int leftMargin = 20;
	//#expand private static int topMargin = %net.ayaslive.miniim.ui.core.list.topMargin%;
	private static int topMargin = 20;
	//#expand private static int rightMargin = %net.ayaslive.miniim.ui.core.list.rightMargin%;
	private static int rightMargin = 20;
	//#expand private static int bottomMargin = %net.ayaslive.miniim.ui.core.list.bottomMargin%;
	private static int bottomMargin = 20;
	
	private final static int RESOLUTION = 8;
	
	/*! \name Action */
	/*@{*/
	private final String defaultCommand;
	/*@}*/
	
	protected List(String title, String defaultCommand) {
		super(title);
		this.defaultCommand = defaultCommand;
	}

	public void setActionListener(ActionListener lis) {
		this.lis = lis;
	}
	
	public final static void setTopMargin(int margin) {
		topMargin = margin;
	}
	
	public final static void setLeftMargin(int margin) {
		leftMargin = margin;
	}
	
	public final static void setRightMargin(int margin) {
		rightMargin = margin;
	}
	
	public final static void setBottomMargin(int margin) {
		bottomMargin = margin;
	}
	
	protected abstract Enumeration getItems();
	
	protected abstract ListItem getListItem(Object obj);
	
	protected abstract String getHint();
	
	/** return -1 when it is unknown */
	protected abstract int getCount();
	
	protected final Object getSelected() {
		if(selectedIndex == -1) {
			return null;
		}
		
		Enumeration e = getItems();
		if(e == null) {
			return null;
		}
		int i = -1;
		Object obj;
		while(e.hasMoreElements()) {
			i++;
			obj = e.nextElement();
			if(i == selectedIndex) {
				return obj;
			}
		}
		return null;
	}
	
	protected void setSelectedIndex(int index) {
		selectedIndex = index;
		vpos = selectedIndex;
	}
	
	protected int getSelectedIndex() {
		return selectedIndex;
	}
	
	/** Key press listener to traverse the list content */
	protected void keyPressed(int keyCode, int gameAction) {
		
		/* if menu is activated then do menu commands */
		if(handleMenu(keyCode, gameAction)) {
			repaint();
			return;
		}
		
		// dispatch selected element events
		if(handleElement(keyCode, gameAction)) {
			repaint();
			return;
		}
		
		/* else traverse the list items and work for menu */
        if (gameAction == GameCanvas.UP) {
        	selectedIndex--;
            if (selectedIndex < 0) {
            	if(continuousScrolling) {
            		selectedIndex = getCount() - 1;
            	} else {
            		selectedIndex = 0; /* stay within limits */
            	}
            }
            if(vpos > selectedIndex) {
            	vpos--;
            	mark(vpos);
            }
            /*----------------------------------------------- repaint only the list and menu */
            repaint( HMARGIN, panelTop, width - HMARGIN - HMARGIN, menuY);
        }
        else if (gameAction == GameCanvas.DOWN) {
        	selectedIndex++;
        	int count = getCount();
            if (count != -1 && selectedIndex >= count) {
            	if(continuousScrolling) {
            		selectedIndex = 0;
            	} else {
            		selectedIndex = count - 1;
            	}
            }
            /*----------------------------------------------- repaint only the list and menu */
            repaint( HMARGIN, panelTop, width - HMARGIN - HMARGIN, menuY);
        }
        else if (gameAction == GameCanvas.FIRE) {
        	this.lis.performAction( defaultCommand);
        	/*----------------------------------------------- repaint only the list and menu */
        	repaint( HMARGIN, panelTop, width - HMARGIN - HMARGIN, menuY);
        }
	}
	
	public boolean handleElement(int keyCode, int gameAction) {
		return false;
	}
	
	/** Rendering */
	/*@{*/
	public void paint(Graphics g) {
		/* Draw the List Items */
		showItems(g);
		if(vpos > 0) {
			// So there are elements that can be scrolled back ..
			// draw an arrow
			// #expand g.setColor(%net.ayaslive.miniim.ui.core.list.indicator%);
			g.setColor(0x006699);
			int x = width - 3 * HMARGIN - RESOLUTION - rightMargin;
			int y = panelTop + topMargin + PADDING + RESOLUTION;
			g.fillTriangle(x + RESOLUTION/2, y,  x + RESOLUTION, y + RESOLUTION, x, y + RESOLUTION);
		}
		
		super.paint(g);
		String hint = getHint();
		if(hint != null && !Menu.isActive() && selectedIndex != -1 && getCount() != 0 ) {
			// #ifndef net.ayaslive.miniim.ui.core.list.draw_menu_at_last
			// #expand g.setColor(%net.ayaslive.miniim.ui.core.list.bg%);
			g.setColor(0xFFFFFF);
			g.setFont(Menu.BASE_FONT);
			// #endif
			g.drawString(hint, halfWidth, height - Menu.PADDING, Graphics.HCENTER | Graphics.BOTTOM);
			/* TODO show "<>"(90 degree rotated) icon to indicate that we can traverse through the list  */
		}
	}
	
	private final void showItems(Graphics g) {
		int i = -1;
		Enumeration e = getItems();
		if(e == null) {
			return;
		}
		Object obj;
		int posY = panelTop + topMargin;
		
		// clear
		// #expand g.setColor(%net.ayaslive.miniim.ui.core.list.bg%);
		g.setColor(0xFFFFFF);
		g.fillRect(leftMargin, panelTop, width, menuY - panelTop);
		
		g.setFont(ListItem.ITEM_FONT);
		
		// mark is supported
		if(markSupported()) {
			i = vpos - 1;
		} else {
			while(e.hasMoreElements()) {
				i++;
				if(i < vpos) {
					e.nextElement();
					/* skip the objects upto vpos */
					continue;
				}
				i--;
				break;
			}
		}
		while(e.hasMoreElements()) {
			i++;
			obj = e.nextElement();
			/* see if selected index is more than the item count */
			if(selectedIndex > i && !e.hasMoreElements()) {
				selectedIndex = i;
			}
			posY += showItem(g, obj, posY, i == selectedIndex);
			if(posY > (menuY - bottomMargin)) {
				if(selectedIndex >= i && vpos < selectedIndex) {
					vpos++;
					/* notify data model about the expected read position .. */
					mark(vpos);
					/* try to draw again */
					showItems(g);
				}
				/* no more place to draw */
				
				// So there are more elements left ..
				// draw an arrow
				// #expand g.setColor(%net.ayaslive.miniim.ui.core.list.indicator%);
				g.setColor(0x006699);
				int x = width - 3 * HMARGIN - RESOLUTION - rightMargin;
				int y = menuY - bottomMargin - PADDING - 2*RESOLUTION;
				g.fillTriangle(x + RESOLUTION/2, y + RESOLUTION,  x + RESOLUTION, y, x, y);
				return;
			}
		}
	}
	
	private final int showItem(Graphics g, Object obj, int y, boolean selected) {
		ListItem li = null;
		if(obj instanceof ListItem) {
			li = (ListItem)obj;
		} else {
			li = getListItem(obj);
		}
		if(li == null)
			return 0;
		int ret = li.paint(g, leftMargin + HMARGIN, y + VMARGIN, width - HMARGIN - HMARGIN - 1 - leftMargin - rightMargin, selected) + VMARGIN + VMARGIN;
		li.free();
		return ret;
	}
	
	protected void mark(int position) {
		// mark the scroll position ..
	}
	
	protected boolean markSupported() {
		return false;
	}
	/*@}*/
}
