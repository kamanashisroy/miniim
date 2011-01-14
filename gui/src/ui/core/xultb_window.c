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
#include "core/xultb_obj_factory.h"
#include "ui/core/xultb_window.h"

static struct xultb_obj_factory*window_factory;
struct xultb_window*create_xultb_window(xultb_str_t*title) {
	struct xultb_window*win = xultb_obj_alloc(window_factory);
	if(title)win->title = *title;
	return win;
}

static void xultb_window_init(struct xultb_window*win, int w, int h) {
#if 0
	if(SharedCanvas.singleInstance == null) {
	  SharedCanvas.singleInstance = new SharedCanvas(w, h);
	} else {
	  throw new RuntimeException("Window already initiated..");
	}
#endif
	/** The width of the list */
	win->width = w;
	win->halfWidth = w/2;

	/** The height of the list */
	/** Menu start position by pixel along Y-axis */
	win->height = h;
	win->menuY = h - xultb_menu_get_base_height();
	win->panelTop = win->TITLE_FONT->get_height(win->TITLE_FONT)+ win->PADDING*2;
}

static void xultb_window_show(struct xultb_window*win) {
	XULTB_CORE_UNIMPLEMENTED();
}

static void xultb_window_show_full(struct xultb_window*win, xultb_str_t*right_option, xultb_str_t*left_option, int left_option_count) {
	xultb_menu_set_menu(right_option, left_option, left_option_count);
	XULTB_CORE_UNIMPLEMENTED();
}

static xultb_bool_t xultb_window_is_showing(struct xultb_window*win) {
	XULTB_CORE_UNIMPLEMENTED();
	return XULTB_TRUE;
}

static void xultb_window_show_title(struct xultb_window*win, struct xultb_graphics*g) {
	/* Cleanup Background */
	// #expand g.setColor(%net.ayaslive.miniim.ui.core.window.titleBg%);
	g->set_color(g, 0x006699);
	g->fill_rect(g, 0, 0, win->width, win->panelTop);
	// #ifdef net.ayaslive.miniim.ui.core.window.titleShadow
	// draw shadow
	// #expand g.setColor(%net.ayaslive.miniim.ui.core.window.titleShadow%);
	g->set_color(g, 0x009900);
	g->draw_line(g, 0, win->panelTop, win->width, win->panelTop);
	// #endif
	/* Write the title */
	// #expand g.setColor(%net.ayaslive.miniim.ui.core.window.titleFg%);
	g->set_color(g, 0xFFFFFF);
	g->set_font(g, win->TITLE_FONT);
	g->draw_string(g, &win->title, win->halfWidth, win->PADDING, XULTB_GRAPHICS_TOP|XULTB_GRAPHICS_HCENTER);
}

static void xultb_window_paint(struct xultb_window*win, struct xultb_graphics*g) {
	xultb_window_show_title(win, g);
	xultb_menu_show(g, win->width, win->height);
}

static int xultb_window_initialize(void*data) {
	struct xultb_window*win = data;
	win->PADDING = 2;
	win->title = xultb_str_create("XulTube");
	win->init = xultb_window_init;
	win->show = xultb_window_show;
	win->show_full = xultb_window_show_full;
	win->is_showing = xultb_window_is_showing;
	win->paint = xultb_window_paint;
	return 0;
}
