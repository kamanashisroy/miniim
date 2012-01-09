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
#include "core/logger.h"
#include "core/xultb_obj_factory.h"
#include "ui/xultb_guicore.h"
#include "ui/core/xultb_window.h"
#include "ui/core/xultb_menu.h"

C_CAPSULE_START


OPP_CB(xultb_window);

static void xultb_window_init(struct xultb_window*win, int w, int h);
static void xultb_window_show(struct xultb_window*win);
static void xultb_window_show_full(struct xultb_window*win
		, struct opp_factory*left_option, xultb_str_t*right_option);
static xultb_bool_t xultb_window_is_showing(struct xultb_window*win);
static void xultb_window_paint(struct xultb_window*win, struct xultb_graphics*g);
static xultb_bool_t xultb_window_handle_event_impl(struct xultb_window*win, void*target, int flags, int key_code, int x, int y);
void xultb_window_set_title(struct xultb_window*win, xultb_str_t*title);

struct opp_vtable_xultb_window vtable_xultb_window = {
	.PADDING = 2,
	.init = xultb_window_init,
	.show = xultb_window_show,
	.show_full = xultb_window_show_full,
	.set_title = xultb_window_set_title,
	.is_showing = xultb_window_is_showing,
	.paint = xultb_window_paint,
	.handle_event = xultb_window_handle_event_impl,
	.oppcb = OPP_CB_FUNC(xultb_window),
};

void xultb_window_set_title(struct xultb_window*win, xultb_str_t*title) {
	win->title = title;
}

static xultb_bool_t xultb_window_handle_event_impl(struct xultb_window*win, void*target, int flags, int key_code, int x, int y) {
	SYNC_LOG(SYNC_VERB, "Handle menu commands\n");
	if(xultb_menu_handle_event(win, target, flags, key_code, x, y)) {
		xultb_guicore_set_dirty(win);
		return 1;
	}
	return 0;
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
	win->menuY = h - 0;//xultb_menu_get_base_height();
	win->panelTop = win->vtable->TITLE_FONT->get_height(win->vtable->TITLE_FONT)+ win->vtable->PADDING*2;
}

static void xultb_window_show(struct xultb_window*win) {
	XULTB_CORE_UNIMPLEMENTED();
}

static void xultb_window_show_full(struct xultb_window*win
		, struct opp_factory*left_option, xultb_str_t*right_option) {
	xultb_menu_set(left_option, right_option);
	win->vtable->show(win);
}

static xultb_bool_t xultb_window_is_showing(struct xultb_window*win) {
	XULTB_CORE_UNIMPLEMENTED();
	return XULTB_TRUE;
}

static void xultb_window_paint_title_impl(struct xultb_window*win, struct xultb_graphics*g) {
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
	g->set_font(g, win->vtable->TITLE_FONT);
	g->draw_string(g, win->title, 0, vtable_xultb_window.PADDING
			, win->width
			, win->height
			, XULTB_GRAPHICS_TOP|XULTB_GRAPHICS_HCENTER);
}

static void xultb_window_paint(struct xultb_window*win, struct xultb_graphics*g) {
	xultb_window_paint_title_impl(win, g);
	xultb_menu_paint(g, win->width, win->height);
}

OPP_CB(xultb_window) {
	struct xultb_window*win = (struct xultb_window*)data;
	switch(callback) {
	case OPPN_ACTION_INITIALIZE:
		memset(win, 0, sizeof(*win));
		opp_vtable_set(win, xultb_window);
		xultb_window_init(win, 200, 400);
		xultb_window_platform_create(win);
		return 0;
	case OPPN_ACTION_FINALIZE:
//		xultb_window_platform_destroy(win);
		break;
	}
	return 0;
}

int xultb_window_system_init() {
	vtable_xultb_window.TITLE_FONT = xultb_font_create();
//	vtable_xultb_window.TITLE_FONT_HEIGHT = vtable_xultb_window.TITLE_FONT.get_height(vtable_xultb_window.TITLE_FONT);
	xultb_window_system_platform_init();
	xultb_menu_system_init();
	return 0;
}

C_CAPSULE_END
