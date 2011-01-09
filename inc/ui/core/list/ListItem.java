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


package net.ayaslive.miniim.ui.core.list;

import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;

public interface ListItem {
	final static Font ITEM_FONT = Font.getFont(Font.FACE_MONOSPACE, Font.STYLE_PLAIN,
			Font.SIZE_SMALL);
	
	final static int FONT_HEIGHT = ITEM_FONT.getHeight();
	final static int PADDING = 3;
	final static int DPADDING = PADDING*2;
	int paint(Graphics g, int x, int y, int width, boolean selected);
	void free();
}
