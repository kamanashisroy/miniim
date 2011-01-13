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
	XULTB_LIST_ITEM_HMARGIN = 3,
	XULTB_LIST_ITEM_VMARGIN = 2,
	XULTB_LIST_ITEM_RESOLUTION = 8,
};

static int xultb_list_show_item(struct xultb_graphics*g, void*data, int y, xultb_bool_t selected) {
	xultb_list_item*li = NULL;
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
	int ret = li.paint(g, leftMargin + HMARGIN, y + VMARGIN, width - HMARGIN - HMARGIN - 1 - leftMargin - rightMargin, selected) + VMARGIN + VMARGIN;
	li.free();
	return ret;
}

static void xultb_list_show_items(struct xultb_graphics*g, struct xultb_list*list) {
	int i = -1;
	Enumeration e = getItems();
	if (e == null) {
		return;
	}
	Object obj;
	int posY = panelTop + topMargin;

	// clear
	// #expand g.setColor(%net.ayaslive.miniim.ui.core.list.bg%);
	g->set_color(0xFFFFFF);
	g->fill_rect(leftMargin, panelTop, width, menuY - panelTop);

	g->set_font(ListItem.ITEM_FONT);

	// mark is supported
	if (markSupported()) {
		i = vpos - 1;
	} else {
		while (e.hasMoreElements()) {
			i++;
			if (i < vpos) {
				e.nextElement();
				/* skip the objects upto vpos */
				continue;
			}
			i--;
			break;
		}
	}
	while (e.hasMoreElements()) {
		i++;
		obj = e.nextElement();
		/* see if selected index is more than the item count */
		if (selectedIndex > i && !e.hasMoreElements()) {
			selectedIndex = i;
		}
		posY += showItem(g, obj, posY, i == selectedIndex);
		if (posY > (menuY - bottomMargin)) {
			if (selectedIndex >= i && vpos < selectedIndex) {
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
			int y = menuY - bottomMargin - PADDING - 2 * RESOLUTION;
			g.fillTriangle(x + RESOLUTION / 2, y + RESOLUTION, x + RESOLUTION,
					y, x, y);
			return;
		}
	}
}

static void xultb_list_paint(struct xultb_graphics*g, struct xultb_list*list) {
    /* Draw the List Items */
	xultb_list_show_items(g, list);
	if (list->vpos > 0) {
		// So there are elements that can be scrolled back ..
		// draw an arrow
		// #expand g.setColor(%net.ayaslive.miniim.ui.core.list.indicator%);
		g->set_color(0x006699);
		int x = width - 3 * HMARGIN - RESOLUTION - rightMargin;
		int y = panelTop + topMargin + PADDING + RESOLUTION;
		g->fill_triangle(x + RESOLUTION / 2, y, x + RESOLUTION, y + RESOLUTION,
				x, y + RESOLUTION);
	}

	super.paint(g);
	String hint = getHint();
	if (hint != null && !Menu.isActive() && selectedIndex != -1 && getCount()
			!= 0) {
		// #ifndef net.ayaslive.miniim.ui.core.list.draw_menu_at_last
		// #expand g.setColor(%net.ayaslive.miniim.ui.core.list.bg%);
		g->set_color(0xFFFFFF);
		g->set_font(Menu.BASE_FONT);
		// #endif
		g->draw_string(hint, halfWidth, height - Menu.PADDING, Graphics.HCENTER
				| Graphics.BOTTOM);
		/* TODO show "<>"(90 degree rotated) icon to indicate that we can traverse through the list  */
	}
}

static xmltb_str_t* xultb_list_get_hint() {
	return NULL;
}

static void xultb_list_set_action_listener(struct xultb_action_listener*lis) {
	return;
}

static obj_factory* xultb_list_get_items() {
	return NULL;
}

static struct xmltb_list_item* xultb_list_get_list_item(void*data) {
	return NULL;
}

static int xultb_list_initialize(void*data) {
	struct xultb_list*list = data;
	list->get_selected_index = xultb_list_get_selected_index;
	list->get_selected = xultb_list_get_selected;
	list->get_items = xultb_list_get_items;
	list->get_hint = xultb_list_get_hint;
	list->set_action_listener = xultb_list_set_action_listener;
	list->set_selected_index = xultb_list_set_selected_index;
	return 1;
}

struct xultb_list*xultb_list_create(xultb_str_t*title, xultb_str_t*default_command) {
	struct xultb_list*list = obj_alloc(xultb_list_factory);
	list->title = *title;
	list->default_command = *default_command;
	return list;
}



