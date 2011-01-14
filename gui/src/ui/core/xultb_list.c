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
#include "ui/core/xultb_list.h"

static struct xmltb_list_item*xultb_list_get_selected() {
	return NULL;
}

static void xultb_list_set_selected_index(int index) {
	return;
}

enum {
	XULTB_LIST_HMARGIN = 3,
	XULTB_LIST_VMARGIN = 2,
	XULTB_LIST_RESOLUTION = 8,
};

static int xultb_list_show_item(struct xultb_list*list, struct xultb_graphics*g, void*data, int y, xultb_bool_t selected) {
	struct xultb_list_item*li = NULL;
#if 0
	if(obj instanceof ListItem) {
	  li = (ListItem)obj;
	} else {
	  li = getListItem(obj);
	}
#else
	li = data;
#endif
	if(li == NULL)
	  return 0;
	int ret = li->paint(li, g, list->leftMargin + XULTB_LIST_HMARGIN, y + XULTB_LIST_VMARGIN, list->win.width - XULTB_LIST_HMARGIN - XULTB_LIST_HMARGIN - 1 - list->leftMargin - list->rightMargin, selected) + XULTB_LIST_VMARGIN + XULTB_LIST_VMARGIN;
	li->free();
	return ret;
}

static void xultb_list_show_items(struct xultb_list*list, struct xultb_graphics*g) {
	int i = -1;
	struct xultb_obj_factory*items = list->get_items();
	void*obj;
	int posY = list->win.panelTop + list->topMargin;

	// sanity check
	if (items == NULL) {
		return;
	}
	// clear
	// #expand g.setColor(%net.ayaslive.miniim.ui.core.list.bg%);
	g->set_color(g, 0xFFFFFF);
	g->fill_rect(g, list->leftMargin, list->win.panelTop, list->win.width, list->win.menuY - list->win.panelTop);

	g->set_font(g, list->ITEM_FONT);

	for (i = list->vpos - 1; obj = obj_get(items,i);i++) {
		/* see if selected index is more than the item count */
		if (list->selected_index > i && !obj_get(items,i)) {
			list->selected_index = i;
		}
		posY += xultb_list_show_item(list, g, obj, posY, i == list->selected_index);
		if (posY > (list->win.menuY - list->bottomMargin)) {
			if (list->selected_index >= i && list->vpos < list->selected_index) {
				list->vpos++;
				/* try to draw again */
				xultb_list_show_items(list, g);
			}
			/* no more place to draw */

			// So there are more elements left ..
			// draw an arrow
			// #expand g.setColor(%net.ayaslive.miniim.ui.core.list.indicator%);
			g->set_color(g, 0x006699);
			int x = list->win.width - 3 * XULTB_LIST_HMARGIN - XULTB_LIST_RESOLUTION - list->rightMargin;
			int y = list->win.menuY - list->bottomMargin - list->win.PADDING - 2 * XULTB_LIST_RESOLUTION;
			g->fill_triangle(g, x + XULTB_LIST_RESOLUTION / 2, y + XULTB_LIST_RESOLUTION, x + XULTB_LIST_RESOLUTION,
					y, x, y);
			return;
		}
	}
}

static void xultb_list_paint(struct xultb_list*list, struct xultb_graphics*g) {
    /* Draw the List Items */
	xultb_list_show_items(list, g);
	if (list->vpos > 0) {
		// So there are elements that can be scrolled back ..
		// draw an arrow
		// #expand g.setColor(%net.ayaslive.miniim.ui.core.list.indicator%);
		g->set_color(g, 0x006699);
		int x = list->win.width - 3 * XULTB_LIST_HMARGIN - XULTB_LIST_RESOLUTION - list->rightMargin;
		int y = list->win.panelTop + list->topMargin + list->win.PADDING + XULTB_LIST_RESOLUTION;
		g->fill_triangle(g, x + XULTB_LIST_RESOLUTION / 2, y, x + XULTB_LIST_RESOLUTION, y + XULTB_LIST_RESOLUTION,
				x, y + XULTB_LIST_RESOLUTION);
	}

	list->win.paint(&list->win, g);
	xultb_str_t* hint = list->get_hint(list);
	if (hint != NULL && !Menu.isActive() && list->selected_index != -1 && list->get_count(list)
			!= 0) {
		// #ifndef net.ayaslive.miniim.ui.core.list.draw_menu_at_last
		// #expand g.setColor(%net.ayaslive.miniim.ui.core.list.bg%);
		g->set_color(g, 0xFFFFFF);
		g->set_font(g, Menu.BASE_FONT);
		// #endif
		g->draw_string(g, hint, list->win.halfWidth, height - Menu.PADDING, 1/*Graphics.HCENTER|Graphics.BOTTOM*/);
		/* TODO show "<>"(90 degree rotated) icon to indicate that we can traverse through the list  */
	}
}

static xultb_str_t* xultb_list_get_hint(struct xultb_list*list) {
	return NULL;
}

static void xultb_list_set_action_listener(struct xultb_action_listener*lis) {
	return;
}

static struct obj_factory* xultb_list_get_items(struct xultb_list*list) {
	return NULL;
}

static struct xmltb_list_item* xultb_list_get_list_item(void*data) {
	return NULL;
}

static int xultb_list_initialize(void*data) {
	struct xultb_list*list = data;
	//list->get_selected_index = xultb_list_get_selected_index;
	list->get_selected = xultb_list_get_selected;
	list->get_items = xultb_list_get_items;
	list->get_hint = xultb_list_get_hint;
	list->set_action_listener = xultb_list_set_action_listener;
	list->set_selected_index = xultb_list_set_selected_index;
	list->paint = xultb_list_paint;
	return 0;
}

static int xultb_list_finalize(void*data) {
	return 0;
}

static int xultb_list_deepcopy(void*data) {
	return 0;
}

static xultb_obj_factory*xultb_list_factory;
struct xultb_list*xultb_list_create(xultb_str_t*title, xultb_str_t*default_command) {
	struct xultb_list*list = xultb_obj_factory_alloc(xultb_list_factory);
	list->title = *title;
	list->default_command = *default_command;
	return list;
}

static int xultb_list_system_init() {
	xultb_list_factory = obj_factory_create(3,10,xultb_list_initialize, xultb_list_finalize, xultb_list_deepcopy);
}

