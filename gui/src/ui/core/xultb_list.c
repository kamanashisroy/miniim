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
#include "ui/core/xultb_list.h"

C_CAPSULE_START

opp_vtable_extern(xultb_window);

static struct xultb_list_item*xultb_list_get_selected(struct xultb_list*list) {
	return NULL;
}

static void xultb_list_set_selected_index(struct xultb_list*list, int index) {
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
	li = (struct xultb_list_item*)data;
#endif
	if(li == NULL)
	  return 0;
	int ret = li->vtable->paint(li, g, list->leftMargin + XULTB_LIST_HMARGIN, y + XULTB_LIST_VMARGIN, list->super_data.width - XULTB_LIST_HMARGIN - XULTB_LIST_HMARGIN - 1 - list->leftMargin - list->rightMargin, selected) + XULTB_LIST_VMARGIN + XULTB_LIST_VMARGIN;
	OPPUNREF(li);
	return ret;
}

static void xultb_list_show_items(struct xultb_list*list, struct xultb_graphics*g) {
	int i = -1;
	struct opp_factory*items = list->vtable->get_items(list);
	void*obj;
	int posY = list->super_data.panelTop + list->topMargin;

	SYNC_LOG(SYNC_VERB, "Iterating items...\n");
	// sanity check
	if (items == NULL) {
		return;
	}
	// clear
	// #expand g.setColor(%net.ayaslive.miniim.ui.core.list.bg%);
	g->set_color(g, 0xFFFFFF);
	g->fill_rect(g, list->leftMargin, list->super_data.panelTop, list->super_data.width, list->super_data.menuY - list->super_data.panelTop);

	g->set_font(g, list->item_font);

	SYNC_LOG(SYNC_VERB, "Iterating items(%d)\n", list->vpos);
	for (i = list->vpos - 1; (obj = opp_indexed_list_get(items,i));i++) {
		/* see if selected index is more than the item count */
		if (list->selected_index > i && !opp_indexed_list_get(items,i)) {
			list->selected_index = i;
		}
		SYNC_LOG(SYNC_VERB, "Showing item\n");
		posY += xultb_list_show_item(list, g, obj, posY, i == list->selected_index);
		if (posY > (list->super_data.menuY - list->bottomMargin)) {
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
			int x = list->super_data.width - 3 * XULTB_LIST_HMARGIN - XULTB_LIST_RESOLUTION - list->rightMargin;
			int y = list->super_data.menuY - list->bottomMargin - vtable_xultb_window.PADDING - 2 * XULTB_LIST_RESOLUTION;
			g->fill_triangle(g, x + XULTB_LIST_RESOLUTION / 2, y + XULTB_LIST_RESOLUTION, x + XULTB_LIST_RESOLUTION,
					y, x, y);
			return;
		}
	}
}

static void xultb_list_paint(struct xultb_list*list, struct xultb_graphics*g) {
	SYNC_LOG(SYNC_VERB, "Drawing list...\n");
	/* Draw the List Items */
	xultb_list_show_items(list, g);
	if (list->vpos > 0) {
		// So there are elements that can be scrolled back ..
		// draw an arrow
		// #expand g.setColor(%net.ayaslive.miniim.ui.core.list.indicator%);
		g->set_color(g, 0x006699);
		int x = list->super_data.width - 3 * XULTB_LIST_HMARGIN - XULTB_LIST_RESOLUTION - list->rightMargin;
		int y = list->super_data.panelTop + list->topMargin + vtable_xultb_window.PADDING + XULTB_LIST_RESOLUTION;
		g->fill_triangle(g, x + XULTB_LIST_RESOLUTION / 2, y, x + XULTB_LIST_RESOLUTION, y + XULTB_LIST_RESOLUTION,
				x, y + XULTB_LIST_RESOLUTION);
	}

	vtable_xultb_window.paint(&list->super_data, g);
	xultb_str_t* hint = list->vtable->get_hint(list);
	if (hint != NULL && !xultb_menu_is_active() && list->selected_index != -1 && list->vtable->get_count(list)
			!= 0) {
		// #ifndef net.ayaslive.miniim.ui.core.list.draw_menu_at_last
		// #expand g.setColor(%net.ayaslive.miniim.ui.core.list.bg%);
		g->set_color(g, 0xFFFFFF);
		g->set_font(g, xultb_menu_get_base_font());
		// #endif
		g->draw_string(g, hint
				, 0
				, 0
				, list->super_data.width
				, list->super_data.height - XULTB_MENU_PADDING
				, XULTB_GRAPHICS_HCENTER|XULTB_GRAPHICS_BOTTOM);
		/* TODO show "<>"(90 degree rotated) icon to indicate that we can traverse through the list  */
	}
}


static void xultb_list_window_paint_wrapper(struct xultb_window*win, struct xultb_graphics*g) {
	struct xultb_list*list = (struct xultb_list*)win;
	xultb_list_paint(list, g);
}

static xultb_str_t* xultb_list_get_hint(struct xultb_list*list) {
	return NULL;
}

static void xultb_list_set_action_listener(struct xultb_action_listener*list) {
	return;
}

static struct opp_factory* xultb_list_get_items(struct xultb_list*list) {
	return &list->_items;
}

static struct xultb_list_item* xultb_list_get_list_item(void*data) {
	return NULL;
}

struct opp_vtable_xultb_list vtable_xultb_list = {
		//.get_selected_index = xultb_list_get_selected_index,
		.get_selected = xultb_list_get_selected,
		.get_items = xultb_list_get_items,
		.get_list_item = xultb_list_get_list_item,
		.get_hint = xultb_list_get_hint,
		.set_action_listener = xultb_list_set_action_listener,
		.set_selected_index = xultb_list_set_selected_index
};

static struct opp_vtable_xultb_window vtable_xultb_window_list;
/*
opp_vtable_define(xultb_list,(
	.get_selected = xultb_list_get_selected,
	.get_items = xultb_list_get_items,
	.get_list_item = xultb_list_get_list_item,
	.get_hint = xultb_list_get_hint,
	.set_action_listener = xultb_list_set_action_listener,
	.set_selected_index = xultb_list_set_selected_index
	)
);
*/

OPP_CB(xultb_list) {
	struct xultb_list*list = (struct xultb_list*)data;
	// do a cleanup
	memset(list, 0, sizeof(struct xultb_list));
	switch(callback) {
	case OPPN_ACTION_INITIALIZE:
		{
			va_list ap;
			if(vtable_xultb_window.oppcb(&list->super_data, OPPN_ACTION_INITIALIZE, NULL, ap)) {
				return -1;
			}
		}
		list->super_data.vtable = &vtable_xultb_window_list;
		opp_vtable_set(list, xultb_list);
		opp_indexed_list_create2(&list->_items, 4);
		list->vpos = 1;
		SYNC_LOG(SYNC_VERB, "Created xultb_list\n");
		return 0;
	case OPPN_ACTION_FINALIZE:
		opp_factory_destroy(&list->_items);
		{
			va_list ap;
			if(vtable_xultb_window.oppcb(&list->super_data, OPPN_ACTION_FINALIZE, NULL, ap)) {
				return -1;
			}
		}
		break;
	}
	return 0;
}

static struct opp_factory xultb_list_factory;
struct xultb_list*xultb_list_create(xultb_str_t*title, xultb_str_t*default_command) {
	struct xultb_list*list = (struct xultb_list*)OPP_ALLOC2(&xultb_list_factory, NULL);
	if(!list) {
		return NULL;
	}
	list->item_font = xultb_font_create();
	if(title)list->super_data.title = *title;
	list->default_command = *default_command;
	return list;
}

int xultb_list_system_init() {
	vtable_xultb_window_list = vtable_xultb_window;
	vtable_xultb_window_list.paint = xultb_list_window_paint_wrapper;
	SYNC_ASSERT(OPP_FACTORY_CREATE(
			&xultb_list_factory
			, 1,sizeof(struct xultb_list)
			, OPP_CB_FUNC(xultb_list)) == 0);
	return 0;
}

C_CAPSULE_END
