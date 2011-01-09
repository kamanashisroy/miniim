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

import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;

import net.ayaslive.miniim.ui.core.Window;

public final class ListItemFactory implements ListItem {
	/** Styling */
	//#ifndef net.ayaslive.miniim.ui.core.list.listitemfactory.fg
	
	// define style
	//#define net.ayaslive.miniim.ui.core.list.listitemfactory.fg='0x000000'
	//#define net.ayaslive.miniim.ui.core.list.listitemfactory.bgHover='0x0099CC'
	//#define net.ayaslive.miniim.ui.core.list.listitemfactory.fgHover='0xFFFFFF'
	//#define net.ayaslive.miniim.ui.core.list.listitemfactory.box='0xCCCCCC'
	//#define net.ayaslive.miniim.ui.core.list.listitemfactory.boxHover='0x006699'
	//#define net.ayaslive.miniim.ui.core.list.listitemfactory.boxHoverInactive='0x999999'
	//#define net.ayaslive.miniim.ui.core.list.listitemfactory.bgShadeHover = '0x00CCFF'
	//#define net.ayaslive.miniim.ui.core.rounded_corner_radius = 4
	//#endif
	
	private String text = null;
	private Image img = null;
	
	private String label = null;
	
	/** wheather we should fill the background when selected */
	private boolean isEditable = true;
	private final static int RESOLUTION = 8;
	
	private boolean truncateTextToFitWidth = false;
	
	/// List
	/*@{*/
	private boolean wrapped = false;
	/*@}*/

	/// Checkbox
	/*@{*/
	private boolean checked = false;
	private boolean isRadio = false;
	/*@}*/
	
	public final static int LABEL = 0;
	public final static int TEXT_INPUT = 1;
	public final static int SELECTION = 2;
	public final static int CHECKBOX = 3;
	private int type = LABEL;
	
	private final static ListItemFactory item = new ListItemFactory();

	private ListItemFactory() {
	}

	public final static ListItemFactory createLabel(String label, Image img) {
		return createLabel(label, img, true, false);
	}
	
	public final static ListItemFactory createLabel(String label, Image img, boolean changeBgOnFocus, boolean truncateTextToFitWidth) {
		item.label = label;
		item.img = img;
		item.isEditable = changeBgOnFocus;
		item.type = LABEL;
		item.truncateTextToFitWidth = truncateTextToFitWidth;
		return item;
	}
	
	public final static ListItemFactory createSelectionBox(String label, String text, boolean editable) {
		item.label = label;
		item.text = text;
		item.isEditable = editable;
		item.type = SELECTION;
		return item;
	}
	
	public final static ListItemFactory createTextInput(String label, String text, boolean wrapped, boolean editable) {
		item.label = label;
		item.text = (text == null)?"":text;
		item.wrapped = wrapped;
		item.isEditable = editable;
		item.type = TEXT_INPUT;
		return item;
	}
	
	public final static ListItemFactory createTextInput(String label, String text) {
		return createTextInput(label, text, false, true);
	}
	
	public final static ListItemFactory createCheckBox(String label, boolean checked, boolean editable) {
		return createCheckBox(label, checked, editable, false);
	}
	
	public final static ListItemFactory createRadioButton(String label, boolean checked, boolean editable) {
		return createCheckBox(label, checked, editable, true);
	}
	
	private final static ListItemFactory createCheckBox(String label, boolean checked, boolean editable, boolean isRadio) {
		item.label = label;
		item.checked = checked;
		item.isEditable = editable;
		item.type = CHECKBOX;
		item.img = null;
		item.isRadio = isRadio;
		return item;
	}
	
	public final int paint(Graphics g, int x, int y, int width, boolean focused) {
		int start, pos, ret, labelWidth, labelHeight, lineCount;
		int imgspacing = 0;
		if(img != null) {
			imgspacing = img.getWidth() + PADDING;
		}
		if(type == CHECKBOX) {
			imgspacing = RESOLUTION + PADDING;
		}
		
		// Write the Label
		labelWidth = labelHeight = start = pos = ret = lineCount = 0;
		if(label != null) {
			
			// #expand g.setColor(%net.ayaslive.miniim.ui.core.list.listitemfactory.fg%);
			g.setColor(0x000000);
			while((pos = Window.wrapNext(label, ITEM_FONT, start, width - imgspacing - DPADDING)) != -1) {
				if(focused && isEditable && type == LABEL) {
					// #expand g.setColor(%net.ayaslive.miniim.ui.core.list.listitemfactory.bgHover%);
					g.setColor(0x0099CC);

					// draw the advanced background
					// #ifdef net.ayaslive.miniim.ui.core.list.listitemfactory.bgShadeHover

					// Do special things if we are in the first line 
					if(start == 0) {
						
						// draw rounded background
						// #expand g.fillRoundRect( x, y + ret, width, FONT_HEIGHT + DPADDING, %net.ayaslive.miniim.ui.core.rounded_corner_radius%, %net.ayaslive.miniim.ui.core.rounded_corner_radius%);
						g.fillRoundRect( x, y + ret, width, FONT_HEIGHT + DPADDING, 4, 4);
	
						// draw the shadow
						// #expand g.setColor(%net.ayaslive.miniim.ui.core.list.listitemfactory.bgShadeHover%);
						g.setColor(0x00CCFF);
						// #expand g.fillRect( x + %net.ayaslive.miniim.ui.core.rounded_corner_radius%/2, y + ret + %net.ayaslive.miniim.ui.core.rounded_corner_radius%/2, width - %net.ayaslive.miniim.ui.core.rounded_corner_radius%, FONT_HEIGHT/2 + PADDING - %net.ayaslive.miniim.ui.core.rounded_corner_radius%);
						g.fillRect( x + 4/2, y + ret + 4/2, width - 4, FONT_HEIGHT/2 + PADDING - 4);
					} else {
						g.fillRect( x, y + ret, width, FONT_HEIGHT + DPADDING);
					}
					
					// #else
//@					
//@					// draw plain background
//@					g.fillRect( x, y + ret, width, FONT_HEIGHT + DPADDING);
					// #endif
					
					// draw text
					// #expand g.setColor(%net.ayaslive.miniim.ui.core.list.listitemfactory.fgHover%);
					g.setColor(0xFFFFFF);
				}
				g.drawString( label.substring(start,pos), x + imgspacing + PADDING, y + ret + PADDING, Graphics.TOP|Graphics.LEFT);
				ret += FONT_HEIGHT + DPADDING;
				start = pos;
				if(start == 0) {
					imgspacing = 0;
				}
				if(truncateTextToFitWidth) {
					break;
				}
			}
			if(type != LABEL && type != CHECKBOX) {
				labelWidth = ITEM_FONT.stringWidth(label);
				if(!wrapped && (labelWidth) > width - DPADDING ) {
					wrapped = false;
				}
				labelHeight = ret;
				labelWidth += DPADDING;
			}
		}
		if(text != null) {
			if(wrapped) {
				
				lineCount = 0;
				if(text.length() != 0) { /* when the string length is Zero */
					/* write the text in the next line */
					start = pos = 0;
					while((pos = Window.wrapNext(text, ITEM_FONT, start, width - DPADDING)) != -1 && lineCount < 3) {
						g.drawString( text.substring(start,pos), x + PADDING, y + ret + PADDING, Graphics.TOP|Graphics.LEFT);
						ret += FONT_HEIGHT + DPADDING;
						start = pos;
						lineCount++;
					}
				}
				/* we are trying to show 3 lines always, not more not less */
				ret += (3-lineCount)*(FONT_HEIGHT + DPADDING);
				labelWidth = 0;
			} else {
				
				int imgWidth = 0;
				if(type == SELECTION) {
					imgWidth = RESOLUTION;
				}

				pos = Window.wrapNext(text, ITEM_FONT, 0, width - labelWidth - DPADDING - imgWidth - DPADDING);
				if(pos != -1) {
					g.drawString( text.substring(0,pos), x + labelWidth + PADDING, y + PADDING, Graphics.TOP|Graphics.LEFT);
					if(pos < text.length()) {
						/* show an image at last to indicate that there are more data .. */
					}
				}
				if(type == SELECTION) {
					drawSelectionBoxIcon(g, x + width - RESOLUTION - PADDING, y + PADDING, focused);
				}
				labelHeight = 0;
			}
		}
		// Draw the image
		if(img != null) {
			g.drawImage( img, x + PADDING, y + PADDING, Graphics.TOP|Graphics.LEFT);
		}
		
		if(type == CHECKBOX) {
			drawCheckBoxIcon(g, x + PADDING, y + PADDING, focused);
		}
		
		// #expand g.setColor(focused?(isEditable?%net.ayaslive.miniim.ui.core.list.listitemfactory.boxHover%:%net.ayaslive.miniim.ui.core.list.listitemfactory.boxHoverInactive%):%net.ayaslive.miniim.ui.core.list.listitemfactory.box%);
		g.setColor(focused?(isEditable?0x006699:0x999999):0xCCCCCC);
		
		/* draw a square */
		if(type != CHECKBOX && (focused || type != LABEL)
				// #ifdef net.ayaslive.miniim.ui.core.list.listitemfactory.bgShadeHover
				&& !(focused && type == LABEL)
				// #endif
				) {
//			#ifdef net.ayaslive.miniim.ui.core.rounded_corner_radius
//			#expand g.drawRoundRect( x + labelWidth, y + labelHeight, width - labelWidth, ret - labelHeight, %net.ayaslive.miniim.ui.core.rounded_corner_radius%, %net.ayaslive.miniim.ui.core.rounded_corner_radius%);
g.drawRoundRect( x + labelWidth, y + labelHeight, width - labelWidth, ret - labelHeight, 4, 4);
//			#else
//@			g.drawRect( x + labelWidth, y + labelHeight, width - labelWidth, ret - labelHeight);
//			#endif
		}
		return ret;
	}

	public void free() {
		item.label = null;
		item.text = null;
		item.wrapped = false;
		item.isEditable = false;
		item.img = null;
		item.truncateTextToFitWidth = false;
	}
	
	public int getType() {
		return type;
	}
	
	private void drawCheckBoxIcon(Graphics g, int x, int y, boolean focused) {
		// draw a box
		// #expand g.setColor(focused?(isEditable?%net.ayaslive.miniim.ui.core.list.listitemfactory.boxHover%:%net.ayaslive.miniim.ui.core.list.listitemfactory.boxHoverInactive%):%net.ayaslive.miniim.ui.core.list.listitemfactory.box%);
		g.setColor(focused?(isEditable?0x006699:0x999999):0xCCCCCC);
		if(isRadio) {
			
			// make the box a little curcular as it is radio button ..
			g.drawRoundRect(x + 1, y + 1, RESOLUTION - 1, RESOLUTION - 1, DPADDING, DPADDING);
		} else {
			
			g.drawRect(x + 1, y + 1, RESOLUTION - 1, RESOLUTION - 1);
		}
		
		// now indicate that it is checked ..
		if(checked) {
			
			if(isRadio) {
				
				g.fillRoundRect(x + 2, y + 2, RESOLUTION - 2, RESOLUTION - 2, DPADDING, DPADDING);
			} else {
				
				// #ifdef net.ayaslive.miniim.ui.core.list.listitemfactory.useTick
	//@			// draw tick
	//@			int bendX = x + RESOLUTION/4 + 1;
	//@			g.drawLine(x + 1, y + RESOLUTION/2, bendX, y + RESOLUTION - 1);
	//@			g.drawLine(bendX, y + RESOLUTION - 2, x + RESOLUTION - 1, y + 1);
				// #else
				g.fillRect(x + 2, y + 2, RESOLUTION - 2, RESOLUTION - 2);
				// #endif
			}
		}
	}
	
	private void drawSelectionBoxIcon(Graphics g, int x, int y, boolean focused) {
		// now indicate that it is checked ..
		/* draw a rectangle */
		// #expand g.setColor(focused?(isEditable?%net.ayaslive.miniim.ui.core.list.listitemfactory.boxHover%:%net.ayaslive.miniim.ui.core.list.listitemfactory.boxHoverInactive%):%net.ayaslive.miniim.ui.core.list.listitemfactory.box%);
		g.setColor(focused?(isEditable?0x006699:0x999999):0xCCCCCC);
		g.fillTriangle(x + RESOLUTION/2, y + RESOLUTION,  x + RESOLUTION, y, x, y);
	}
}
