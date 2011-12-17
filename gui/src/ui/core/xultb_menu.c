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

#include "config.h"
#include "opp/opp_salt.h"
#include "opp/opp_factory.h"
#include "opp/opp_indexed_list.h"
#include "ui/core/xultb_menu.h"

struct xultb_font*xultb_menu_get_base_font() {
	XULTB_CORE_UNIMPLEMENTED();
	return NULL;
}

int xultb_menu_get_base_height() {
	XULTB_CORE_UNIMPLEMENTED();
	return 0;
}

static int menu_is_active = 0;
static xultb_str_t SELECT;
static xultb_str_t CANCEL;
static xultb_str_t rightOption; /* < will be displayed when menu is inactive */
static struct opp_factory menuOptions;

static int menuMaxWidth = -1;
static int menuMaxHeight = -1;


static int BASE_FONT_HEIGHT;
static int TOWER_MENU_ITEM_HEIGHT;
static int TOWER_FONT_HEIGHT;
static xultb_font_t*TOWER_FONT;
static xultb_font_t*BASE_FONT;

static int BASE_HEIGHT;

//private static boolean menuIsActive = false;
static int currentlySelectedIndex = 0;



static void xultb_menu_draw_base(struct xultb_graphics* g, int width, int height, xultb_str_t*left, xultb_str_t*right) {
	/* draw the background of the menu */
	// #expand g.setColor(%net.ayaslive.miniim.ui.core.menu.bgBase%);
	g->set_color(g, 0x006699);
	g->fill_rect(g, 0, height - BASE_HEIGHT, width, BASE_HEIGHT);

	// #ifdef net.ayaslive.miniim.ui.core.menu.baseShadow
	// draw shadow
	// #expand g.setColor(%net.ayaslive.miniim.ui.core.menu.baseShadow%);
	g->set_color(g, 0x006699);
	g->draw_line(g, 0, height - BASE_HEIGHT, width, height - BASE_HEIGHT);
	// #endif

	/* draw left and right menu options */
	g->set_font(g, BASE_FONT);
	// #expand g.setColor(%net.ayaslive.miniim.ui.core.menu.fgBase%);
	g->set_color(g, 0xFFFFFF);

	if(left && left->len) {
		g->draw_string(g, left, XULTB_MENU_PADDING, 0, 1000, height - XULTB_MENU_PADDING, XULTB_GRAPHICS_LEFT
				| XULTB_GRAPHICS_BOTTOM);
	}
	if(right && right->len) {
		g->draw_string(g, right, width - XULTB_MENU_PADDING, 0, width, height - XULTB_MENU_PADDING,
				XULTB_GRAPHICS_RIGHT | XULTB_GRAPHICS_BOTTOM);
	}
}


static void xultb_menu_precalculate() {
	int currentWidth = 0;

	/* we'll simply check each option and find the maximal width */
//	for (int i = 0; i < menuOptions.length; i++) {
	int i = 0;
	for (;;i++) {
		xultb_str_t*cmd;
		opp_at_ncode(cmd, &menuOptions, i,
			currentWidth = TOWER_FONT->substring_width(TOWER_FONT, cmd, 0, cmd->len);
			if (currentWidth > menuMaxWidth) {
				menuMaxWidth = currentWidth; /* update */
			}
			menuMaxHeight += TOWER_FONT_HEIGHT + 2*XULTB_MENU_PADDING; /*
													 * for a current menu
													 * option
													 */
		) else {
			break;
		}
	}
	menuMaxWidth += 2 * XULTB_MENU_PADDING; /* XULTB_MENU_PADDING from left and right */
}

static void xultb_menu_draw_tower(struct xultb_graphics*g, int width, int height,
			int selectedOptionIndex) {

	/* draw menu options */
	if (!OPP_FACTORY_USE_COUNT(&menuOptions)) {
		return;
	}

	/* check out the max width of a menu (for the specified menu font) */
	if(menuMaxWidth == -1) {
		xultb_menu_precalculate();
	}
	/* Tower top position */
	int menuOptionY = height - BASE_HEIGHT - menuMaxHeight - 1;

	/* now we know the bounds of active menu */

	/* draw active menu's background */
	// #expand g.setColor(%net.ayaslive.miniim.ui.core.menu.bg%);
	g->set_color(g, 0xFFFFFF);
	g->fill_rect(g, 0/* x */, menuOptionY/* y */, menuMaxWidth, menuMaxHeight);
	/* draw border of the menu */
	// #expand g.setColor(%net.ayaslive.miniim.ui.core.menu.borderTower%); // gray
	g->set_color(g, 0xCCCCCC); // gray
	g->draw_rect(g, 0/* x */, menuOptionY/* y */, menuMaxWidth, menuMaxHeight);

	/* draw menu options (from up to bottom) */
	g->set_font(g, TOWER_FONT);

	int i = 0, j = 0;
	for (;;i++) {
		xultb_str_t*cmd;
		opp_at_ncode(cmd, &menuOptions, i,
		if (j != selectedOptionIndex) { /* draw unselected menu option */
			// #expand g.setColor(%net.ayaslive.miniim.ui.core.menu.fg%);
			g->set_color(g, 0x000000);
		} else { /* draw selected menu option */
			/* draw a background */
			// #expand g.setColor(%net.ayaslive.miniim.ui.core.menu.bgHover%);
			g->set_color(g, 0x0099CC);
			g->fill_rect(g, 0, menuOptionY, menuMaxWidth, TOWER_MENU_ITEM_HEIGHT);
			// #expand g.setColor(%net.ayaslive.miniim.ui.core.menu.borderHover%);
			g->set_color(g, 0x006699);
			g->draw_rect(g, 0, menuOptionY, menuMaxWidth, TOWER_MENU_ITEM_HEIGHT);
			/**
			 * The simplest way to separate selected menu option is by
			 * drawing it with different color. However, it also may be
			 * painted as underlined text or with different background
			 * color.
			 */
			// #expand g.setColor(%net.ayaslive.miniim.ui.core.menu.fgHover%);
			g->set_color(g, 0xFFFFFF);
		}

		menuOptionY += XULTB_MENU_PADDING;
		g->draw_string(g, cmd, XULTB_MENU_PADDING, menuOptionY, 1000, 1000,
				XULTB_GRAPHICS_LEFT | XULTB_GRAPHICS_TOP);

		menuOptionY += XULTB_MENU_PADDING + TOWER_FONT_HEIGHT;
		j++;
		) else {
			break;
		}
	}
}

void xultb_menu_paint(struct xultb_graphics*g, int width, int height) {
	if(menu_is_active) {
		xultb_menu_draw_base(g, width, height, &SELECT, &CANCEL);
		xultb_menu_draw_tower(g, width, height, currentlySelectedIndex);
	} else {
		xultb_str_t*cmd;
		opp_at_ncode(cmd, &menuOptions, 0,
			xultb_menu_draw_base(g, width, height, cmd, &rightOption);
		) else {
			xultb_menu_draw_base(g, width, height, NULL, &rightOption);
		}
	}
	return;
}

xultb_bool_t xultb_menu_is_active() {
	XULTB_CORE_UNIMPLEMENTED();
	return XULTB_FALSE;
}

int xultb_menu_system_init() {
	SYNC_ASSERT(opp_indexed_list_create2(&menuOptions, 16) == 0);
	TOWER_FONT = xultb_font_create(); // Font.getFont(Font.FACE_SYSTEM, Font.STYLE_PLAIN, Font.SIZE_SMALL);
	BASE_FONT = xultb_font_create(); // Font.getFont(Font.FACE_SYSTEM, Font.STYLE_BOLD, Font.SIZE_SMALL);
	TOWER_FONT_HEIGHT = TOWER_FONT->get_height(TOWER_FONT);
	BASE_FONT_HEIGHT = BASE_FONT->get_height(BASE_FONT);
	TOWER_MENU_ITEM_HEIGHT = TOWER_FONT_HEIGHT + 2*XULTB_MENU_PADDING;
	BASE_HEIGHT = BASE_FONT_HEIGHT + 2*XULTB_MENU_PADDING;
	SELECT = xultb_str_create("Select");
	CANCEL = xultb_str_create("Cancel");
	rightOption = xultb_str_create("");
	return 0;
}


