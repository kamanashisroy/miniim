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

import javax.microedition.lcdui.*;
import javax.microedition.lcdui.game.*;

/**
 * This code is inspired by
 * http://wiki.forum.nokia.com/index.php/How_to_create_an_advanced_menu_in_Java_ME_using_the_low-level_graphical_API
 * 
 */

void menu {

	/** Styling */
	//#ifndef net.ayaslive.miniim.ui.core.menu.bg
	
	// define style
	//#define net.ayaslive.miniim.ui.core.menu.bg='0xFFFFFF'
	//#define net.ayaslive.miniim.ui.core.menu.fg='0x000000'
	//#define net.ayaslive.miniim.ui.core.menu.bgBase='0x006699'
	//#define net.ayaslive.miniim.ui.core.menu.fgBase='0xFFFFFF'
	//#define net.ayaslive.miniim.ui.core.menu.bgHover='0x0099CC'
	//#define net.ayaslive.miniim.ui.core.menu.fgHover='0xFFFFFF'
	//#define net.ayaslive.miniim.ui.core.menu.borderTower='0xCCCCCC'
	//#define net.ayaslive.miniim.ui.core.menu.borderHover='0x006699'
	//#define net.ayaslive.miniim.ui.core.menu.baseShadow='0x006699'
	
	//#endif
		
	/** 
	 * \name Change Menu Text
	 * Give the ability to change the texts used in Menu operation.
	 * This is needed basically for multilanguage support.
	 */
	/*@{*/
	private static String SELECT = "Select";
	private static String CANCEL = "Cancel";
	private static String MENU = "Menu";
	/** give them public access */
	public final static void setStrings(String menu, String select, String cancel) {
		Menu.MENU = menu;
		Menu.SELECT = select;
		Menu.CANCEL = cancel;
	}
	/*@}*/
	
	final static int PADDING = 3; /* < just like in CSS */
	
	private final static Font TOWER_FONT = Font.getFont(Font.FACE_SYSTEM, Font.STYLE_PLAIN,
			Font.SIZE_SMALL);
	final static Font BASE_FONT = Font.getFont(Font.FACE_SYSTEM, Font.STYLE_BOLD,
			Font.SIZE_SMALL);
	
	private final static int TOWER_FONT_HEIGHT = TOWER_FONT.getHeight();
	private final static int BASE_FONT_HEIGHT = BASE_FONT.getHeight();
	private final static int TOWER_MENU_ITEM_HEIGHT = TOWER_FONT_HEIGHT + 2*PADDING;
	final static int BASE_HEIGHT = BASE_FONT_HEIGHT + 2*PADDING;

	//#ifndef polish.key.leftsoftkey
	//#define polish.key.leftsoftkey = -6
	//#endif
	
	//#ifndef polish.key.rightsoftkey
	//#define polish.key.rightsoftkey = -7
	//#endif
 
	//#expand private static int LEFT_SOFTKEY_CODE = %polish.key.leftsoftkey%;
	private static int LEFT_SOFTKEY_CODE = -6;
	
	//#expand private static int RIGHT_SOFTKEY_CODE = %polish.key.rightsoftkey%;
	private static int RIGHT_SOFTKEY_CODE = -7;

	
	private int menuMaxWidth = -1;
	private int menuMaxHeight = -1;
	
	private static boolean menuIsActive = false;
	private int currentlySelectedIndex = 0;
	
	final static Menu self = new Menu();

	private Menu() {
	}
	
	public static Menu getInstance() {
		return self;
	}

	/** @group configurable part */
	/*@{*/
	private String rightOption = null; /* < will be displayed when menu is inactive */
	private String[] menuOptions = null;
	/**
	 * Creates a new instance of Menu.
	 */
	public synchronized void setMenu(String rightOption, String[] menuOptions) {
		this.rightOption = rightOption;
		this.menuOptions = menuOptions;
		revalidate();
	}
	
	public synchronized void setRightOption(String rightOption) {
		this.rightOption = rightOption;
		// no revalidation needed
	}
	
	public synchronized void setLeftOption(int index, String value) {
		this.menuOptions[index] = value;
		revalidate();
	}
	/*@}*/
	
	/** Get the menu status */
	static boolean isActive() {
		return menuIsActive;
	}
	
	synchronized final void show(Graphics g, int width, int  height) {
		if(menuIsActive) {
			drawBase(g, width, height, SELECT, CANCEL);
			drawTower(g, width, height, currentlySelectedIndex);
		} else {
			drawBase(g, width, height, getFirstOption(), rightOption);
		}
	}
	
	private final String getFirstOption() {
		if(menuOptions == null) {
			return null;
		}
		String ret = null;
		for(int i=0, j=0; i<menuOptions.length;i++) {
			if(menuOptions[i] != null) {
				ret = menuOptions[i];
				j++;
				if(j > 1) {
					return MENU;
				}
			}
		}
		return ret;
	}
	
	private void revalidate() {
		menuMaxWidth = -1;
		menuMaxHeight = -1;
		currentlySelectedIndex = 0;
	}
	
	private final void drawBase(Graphics g, int width, int height, String left, String right) {
		/* draw the background of the menu */
		// #expand g.setColor(%net.ayaslive.miniim.ui.core.menu.bgBase%);
		g.setColor(0x006699);
		g.fillRect(0, height - BASE_HEIGHT, width, BASE_HEIGHT);

		// #ifdef net.ayaslive.miniim.ui.core.menu.baseShadow
		// draw shadow
		// #expand g.setColor(%net.ayaslive.miniim.ui.core.menu.baseShadow%);
		g.setColor(0x006699);
		g.drawLine(0, height - BASE_HEIGHT, width, height - BASE_HEIGHT);
		// #endif
		
		/* draw left and right menu options */
		g.setFont(BASE_FONT);
		// #expand g.setColor(%net.ayaslive.miniim.ui.core.menu.fgBase%);
		g.setColor(0xFFFFFF);
		
		if(left!= null) {
			g.drawString(left, PADDING, height - PADDING, Graphics.LEFT
					| Graphics.BOTTOM);
		}
		if(right != null) {
			g.drawString(right, width - PADDING, height - PADDING,
					Graphics.RIGHT | Graphics.BOTTOM);
		}
	}

	private final void drawTower(Graphics g, int width, int height,
			int selectedOptionIndex) {
		
		/* draw menu options */
		if (menuOptions == null) {
			return;
		}

		/* check out the max width of a menu (for the specified menu font) */
		if(menuMaxWidth == -1) {
			precalculate();
		}
		/* Tower top position */
		int menuOptionY = height - BASE_HEIGHT - menuMaxHeight - 1;
		
		/* now we know the bounds of active menu */
		
		/* draw active menu's background */
		// #expand g.setColor(%net.ayaslive.miniim.ui.core.menu.bg%);
		g.setColor(0xFFFFFF);
		g.fillRect(0/* x */, menuOptionY/* y */, menuMaxWidth, menuMaxHeight);
		/* draw border of the menu */
		// #expand g.setColor(%net.ayaslive.miniim.ui.core.menu.borderTower%); // gray
		g.setColor(0xCCCCCC); // gray
		g.drawRect(0/* x */, menuOptionY/* y */, menuMaxWidth, menuMaxHeight);

		/* draw menu options (from up to bottom) */
		g.setFont(TOWER_FONT);

		for (int i = 0, j = 0; i < menuOptions.length; i++) {
			if(menuOptions[i] == null) {
				continue;
			}
			if (j != selectedOptionIndex) { /* draw unselected menu option */
				// #expand g.setColor(%net.ayaslive.miniim.ui.core.menu.fg%);
				g.setColor(0x000000);
			} else { /* draw selected menu option */
				/* draw a background */
				// #expand g.setColor(%net.ayaslive.miniim.ui.core.menu.bgHover%);
				g.setColor(0x0099CC);
				g.fillRect(0, menuOptionY, menuMaxWidth, TOWER_MENU_ITEM_HEIGHT);
				// #expand g.setColor(%net.ayaslive.miniim.ui.core.menu.borderHover%);
				g.setColor(0x006699);
				g.drawRect(0, menuOptionY, menuMaxWidth, TOWER_MENU_ITEM_HEIGHT);
				/**
				 * The simplest way to separate selected menu option is by
				 * drawing it with different color. However, it also may be
				 * painted as underlined text or with different background
				 * color.
				 */
				// #expand g.setColor(%net.ayaslive.miniim.ui.core.menu.fgHover%);
				g.setColor(0xFFFFFF);
			}

			menuOptionY += PADDING;
			g.drawString(menuOptions[i], PADDING, menuOptionY,
					Graphics.LEFT | Graphics.TOP);

			menuOptionY += PADDING + TOWER_FONT_HEIGHT;
			j++;
		}
	}
	
	private final void precalculate() {
		int currentWidth = 0;

		/* we'll simply check each option and find the maximal width */
		for (int i = 0; i < menuOptions.length; i++) {
			if(menuOptions[i] == null) {
				continue;
			}
			currentWidth = TOWER_FONT.stringWidth(menuOptions[i]);
			if (currentWidth > menuMaxWidth) {
				menuMaxWidth = currentWidth; /* update */
			}
			menuMaxHeight += TOWER_FONT_HEIGHT + 2*PADDING; /*
													 * for a current menu
													 * option
													 */
		}
		menuMaxWidth += 2 * PADDING; /* padding from left and right */
	}

	synchronized boolean keyPressed(int keyCode, int gameAction) {
		int length = 0;
		String firstOption = null;
		String selectedOption = null;
		if(menuOptions != null) {
			for(int i=0; i<menuOptions.length;i++) {
				if(menuOptions[i] != null) {
					if(length == 0) {
						firstOption = menuOptions[i];
					}
					if(length == currentlySelectedIndex) {
						selectedOption = menuOptions[i];
					}
					length++;
				}
			}
		}
		if (menuIsActive) {
            if (keyCode == RIGHT_SOFTKEY_CODE) {
                menuIsActive = false;
            } else if (gameAction == GameCanvas.UP) {
                currentlySelectedIndex--;
                if (currentlySelectedIndex < 0) {
                    currentlySelectedIndex = 0; /* stay within limits */
                }
            }
            else if (gameAction == GameCanvas.DOWN) {
                currentlySelectedIndex++;
                if (currentlySelectedIndex >= length) {
                    currentlySelectedIndex = length - 1; /* stay within limits */
                }
            }
            else if (gameAction == GameCanvas.FIRE || keyCode == LEFT_SOFTKEY_CODE) {
            	if(SharedCanvas.singleInstance.win.lis != null) {
            		SharedCanvas.singleInstance.win.lis.performAction(selectedOption);
            	}
                menuIsActive = false;
            }
        }
        else {
            /* check if the "Options" or "Exit" buttons were pressed */
            if (keyCode == LEFT_SOFTKEY_CODE) { // "Options" pressed
            	if(menuOptions == null) {
            		/* This is not menu action */
            		return false;
            	}
            	if(length == 1) {
            		/* this is direct action */
            		if(SharedCanvas.singleInstance.win.lis != null) {
                		SharedCanvas.singleInstance.win.lis.performAction(firstOption);
                	}
            	} else {
            		currentlySelectedIndex = 0;
            		menuIsActive = true;
            	}
            }
            else if (keyCode == RIGHT_SOFTKEY_CODE) {
            	if(SharedCanvas.singleInstance.win.lis != null) {
            		SharedCanvas.singleInstance.win.lis.performAction(rightOption);
            	}
            } else {
            	/* This is not menu action */
            	return false;
            }
        }
		return true; /* menu action has done something, so the action is digested */
	}
}
