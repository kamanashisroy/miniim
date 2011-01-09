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

package net.ayaslive.miniim.ui.core;

import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;

public abstract class Window {
	/** Styling */
	//#ifndef net.ayaslive.miniim.ui.core.window.titleFg
	
	// define style
	//#define net.ayaslive.miniim.ui.core.window.titleBg='0x006699'
	//#define net.ayaslive.miniim.ui.core.window.titleFg='0xFFFFFF'
	//#define net.ayaslive.miniim.ui.core.window.titleShadow='0x009900'
	//#endif
	
	public final static int PADDING = 2;
	/** The title of the window */
	protected String title;
	
	public final static Font TITLE_FONT = Font.getFont(Font.FACE_SYSTEM, Font.STYLE_BOLD,
			Font.SIZE_SMALL);
	
	/** The width of the list */
	static int width;
	static int halfWidth;

	/** The height of the list */
	/** Menu start position by pixel along Y-axis */
	static int height;
	static int menuY;
	static int panelTop;
	
	protected ActionListener lis = null;
	
	/** Static global fields */
	static boolean isDestroying = false;
	public static Display dis = null;
		
	protected Window(String title) {
		this.title = title;
	}
	
	public synchronized final static void init(int w, int h) {
		if(SharedCanvas.singleInstance == null) {
			SharedCanvas.singleInstance = new SharedCanvas(w, h);
		} else {
			throw new RuntimeException("Window already initiated..");
		}
		/** The width of the list */
		width = w;
		halfWidth = w/2;

		/** The height of the list */
		/** Menu start position by pixel along Y-axis */
		height = h;
		menuY = h - Menu.BASE_HEIGHT;
		panelTop = TITLE_FONT.getHeight()+PADDING*2;
	}
	
	public void show() {
		SharedCanvas.singleInstance.show(this);
	}
	
	protected void show(String rightOption, String[] leftOption) {
		Menu.self.setMenu(rightOption, leftOption);
		SharedCanvas.singleInstance.show(this);
	}
	
	public final boolean isShowing() {
		return (SharedCanvas.singleInstance.win == this);
	}
	
	protected void keyPressed(int keyCode, int gameAction) {
		/* if menu is activated then do menu commands */
		if(handleMenu(keyCode, gameAction)) {
			repaint();
		}
	}
	
	public final void repaint() {
		repaint(0, 0, width, height);
	}
	
	public final void repaint(int x, int y, int width, int height) {
		if(SharedCanvas.singleInstance.win == this) {
			SharedCanvas.singleInstance.repaint(x, y, width, height);
		}
	}
	
	/** Key press listener to traverse the list content */
	protected final boolean handleMenu(int keyCode, int gameAction) {
		/* if menu is activated then do menu commands */
		return Menu.self.keyPressed(keyCode, gameAction);
	}
	
	public void setTitle(String title) {
		this.title = title;
	}
	
	public String getTitle() {
		return title;
	}
	
	public void paint(Graphics g) {
		/* Draw the Title Bar */
		showTitle(g);
		/* Draw the Status or Menu Bar */
		Menu.self.show(g, width, height);
	}
	
	/*!
	 * \xxx causing flicker problem
	protected void keyRepeated(int keyCode) {
		keyPressed(keyCode);
	}*/
	
	private final void showTitle(Graphics g) {
		/* Cleanup Background */
		// #expand g.setColor(%net.ayaslive.miniim.ui.core.window.titleBg%);
		g.setColor(0x006699);
		g.fillRect(0, 0, width, panelTop);
		// #ifdef net.ayaslive.miniim.ui.core.window.titleShadow
		// draw shadow
		// #expand g.setColor(%net.ayaslive.miniim.ui.core.window.titleShadow%);
		g.setColor(0x009900);
		g.drawLine(0, panelTop, width, panelTop);
		// #endif
		/* Write the title */
		// #expand g.setColor(%net.ayaslive.miniim.ui.core.window.titleFg%);
		g.setColor(0xFFFFFF);
		g.setFont(TITLE_FONT);
		g.drawString( title, halfWidth, PADDING, Graphics.TOP|Graphics.HCENTER);
	}
	
	public final static boolean isDestroying() {
		return isDestroying;
	}

	public final static void destroyApp() {
		isDestroying = true;
	}

	public final static int wrapNext(String str, Font font, int pos, int width) {
		int i = pos,start = pos;
		if(str == null || font == null || width <= 0 ) {
			return -1;
		}
		int len = str.length();
		if(pos == len) {
			return -1;
		}
		while (true) {
		    while (i < len && str.charAt (i) > ' ')
			i++;
		    int w = font.substringWidth(str, start, i - start);
		    if (pos == start) {
	            if (w > width) {
	                while (font.substringWidth (str, start, (--i - start )) > width);
	                pos = i;
	                break;
	            }
	        }
	
	        if (w <= width) pos = i;
	
		    if (w > width || i >= len || str.charAt(i) == '\n') break;
		    i++;
		}
		pos = pos >= len ? pos : ++pos;
		return pos;
	}

	public final static void pushBalloon(String message, Image img, int hash, long timeout) throws OutOfMemoryError {
		SharedCanvas.singleInstance.pushBalloon(message, img, hash, timeout);
	}
	
	public final static void pushBalloon(String message, Image img) throws OutOfMemoryError {
		SharedCanvas.singleInstance.pushBalloon(message, img, -1, 1000);
	}
	
	public final static Window getCurrent() {
		return SharedCanvas.singleInstance.win;
	}
}
