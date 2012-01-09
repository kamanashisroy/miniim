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
#include "opp/opp_salt.h"
#include "ui/core/xultb_list.h"
#include "ui/xultb_gui_input.h"

C_CAPSULE_START

opp_vtable_extern(xultb_window);

static void*xultb_list_get_selected(struct xultb_list*list) {
	struct opp_factory*items = list->vtable->get_items(list);
	if(!items) {
		return NULL;
	}
	return opp_indexed_list_get(items, list->selected_index);
}

static void xultb_list_set_selected_index(struct xultb_list*list, int index) {
	list->selected_index = index;
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
	if(list->vtable->get_list_item) {
		li = list->vtable->get_list_item(list, data);
	} else {
		li = (struct xultb_list_item*)data;
	}
#endif
	if(li == NULL)
	  return 0;
	li->focused = selected;
	int ret = li->vtable->paint(li, g, list->leftMargin + XULTB_LIST_HMARGIN
			, y + XULTB_LIST_VMARGIN
			, list->super_data.width - XULTB_LIST_HMARGIN - XULTB_LIST_HMARGIN - 1 - list->leftMargin - list->rightMargin
			, selected) + XULTB_LIST_VMARGIN + XULTB_LIST_VMARGIN;
	if(list->vtable->get_list_item) {
		OPPUNREF(li);
	}
	return ret;
}

static void xultb_list_show_items(struct xultb_list*list, struct xultb_graphics*g) {
	int i = -1;
	struct opp_factory*items = list->vtable->get_items(list);
	void*obj;
	int posY = list->super_data.panelTop + list->topMargin;

	GUI_LOG("Iterating items...\n");
	// sanity check
	if (items == NULL) {
		return;
	}
	// clear
	// #expand g.setColor(%net.ayaslive.miniim.ui.core.list.bg%);
	g->set_color(g, 0xFFFFFF);
	g->fill_rect(g, list->leftMargin, list->super_data.panelTop, list->super_data.width, list->super_data.menuY - list->super_data.panelTop);

	g->set_font(g, list->item_font);

	if (list->selected_index > OPP_FACTORY_USE_COUNT(items)) {
		list->selected_index = 0;
	}
	GUI_LOG("Iterating items(%d)\n", list->vpos);
	for (i = list->vpos - 1;;i++) {
		int break_here = 0;
		opp_at_ncode(obj, items, i,
			/* see if selected index is more than the item count */
			GUI_LOG("Showing item\n");
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
				GUI_LOG("No more place to draw\n");
				break_here = 1;
			}
		) else {
			break;
		}
		if(break_here) {
			break;
		}
	}
}

static void xultb_list_paint(struct xultb_list*list, struct xultb_graphics*g) {
	GUI_LOG("Drawing list...\n");
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


static xultb_bool_t xultb_list_window_handle_event_wrapper(struct xultb_window*win, void*target, int flags, int key_code, int x, int y) {
	struct xultb_list*list = (struct xultb_list*)win;
	GUI_INPUT_LOG("Handle menu commands\n");
	if(vtable_xultb_window.handle_event(win, target, flags, key_code, x, y)) {
		return XULTB_TRUE;
	}

	GUI_INPUT_LOG("Handle menu commands2\n");
	// dispatch selected element events
	if(list->vtable->handle_item && list->vtable->handle_item(list, target, flags, key_code, x, y)) {
		return XULTB_TRUE;
	}
	GUI_INPUT_LOG("So the target is list item\n");
	if(flags & XULTB_INPUT_SCREEN_EVENT) {
		struct opp_factory*items = list->vtable->get_items(list);
		int i;
		for(i=0;items && i>=0;i++) {
			void*obj;
			opp_at_ncode(obj, items, i,
				if(obj == target) {
					GUI_INPUT_LOG("let us make it selected: %d\n", i);
					list->selected_index = i;
					xultb_guicore_set_dirty(&list->super_data); // may be we should refresh partial
					i = -2;
				}
			) else {
				break;
			}
		}
	} else {
		int consumed = 0;
		GUI_INPUT_LOG("Try to edit with keycode:%d, selected index:%d\n", key_code, list->selected_index);
		// if it is keyboard event then perform keyboard tasks
		void*obj = list->vtable->get_selected(list);
		if(obj) {
			struct xultb_list_item*item = list->vtable->get_list_item(list, obj);
			if(item) {
				if(item->vtable && item->vtable->do_edit) {
					consumed = item->vtable->do_edit(item, flags, key_code, x, y);
				}
				OPPUNREF(item);
			}
			OPPUNREF(obj);
		}
		if(consumed) {
			xultb_guicore_set_dirty(win); // TODO tell it to refresh only a portion ..
			return XULTB_TRUE;
		}
		key_code = x?x:key_code; // handle arrow keys ..
	}
	/* else traverse the list items and work for menu */
	if (key_code == XULTB_INPUT_KEY_UP) {
		list->selected_index--;
		if (list->selected_index < 0) {
			if (list->continuous_scrolling) {
				list->selected_index = list->vtable->get_count(list) - 1;
			} else {
				list->selected_index = 0; /* stay within limits */
			}
		}
		if (list->vpos > list->selected_index) {
			list->vpos--;
#if 0
			mark(list->vpos);
#endif
		}
		/*----------------------------------------------- repaint only the list and menu */
		xultb_guicore_set_dirty2(win, XULTB_LIST_HMARGIN, win->panelTop
				, win->width - XULTB_LIST_HMARGIN - XULTB_LIST_HMARGIN, win->menuY);
	} else if (key_code == XULTB_INPUT_KEY_DOWN) {
		list->selected_index++;
		int count = list->vtable->get_count(list);
		if (count != -1 && list->selected_index >= count) {
			if (list->continuous_scrolling) {
				list->selected_index = 0;
			} else {
				list->selected_index = count - 1;
			}
		}
		/*----------------------------------------------- repaint only the list and menu */
		xultb_guicore_set_dirty2(win, XULTB_LIST_HMARGIN
				, win->panelTop, win->width - XULTB_LIST_HMARGIN - XULTB_LIST_HMARGIN, win->menuY);
	} else if (key_code == XULTB_INPUT_KEY_ENTER) {
		if(list->super_data.lis) {
			list->super_data.lis->perform_action(list->super_data.lis->cb_data, list->default_command/*target*/); // should not it be target !
			/*----------------------------------------------- repaint only the list and menu */
			xultb_guicore_set_dirty2(win, XULTB_LIST_HMARGIN
					, win->panelTop, win->width - XULTB_LIST_HMARGIN - XULTB_LIST_HMARGIN, win->menuY);
		}
	}
	return XULTB_TRUE;
}

static void xultb_list_window_paint_wrapper(struct xultb_window*win, struct xultb_graphics*g) {
	struct xultb_list*list = (struct xultb_list*)win;
	xultb_list_paint(list, g);
}

static xultb_str_t* xultb_list_get_hint(struct xultb_list*list) {
	return NULL;
}

static void xultb_list_set_action_listener(struct xultb_list*list, struct xultb_action_listener*lis) {
	if(lis)list->super_data.lis = lis;
}

static struct opp_factory* xultb_list_get_items(struct xultb_list*list) {
	return &list->_items;
}

#if 0
static struct xultb_list_item* xultb_list_get_list_item(struct xultb_list*list, void*data) {
	return NULL;
}
#endif

OPP_CB(xultb_list);
struct opp_vtable_xultb_list vtable_xultb_list = {
		//.get_selected_index = xultb_list_get_selected_index,
		.get_selected = xultb_list_get_selected,
		.get_items = xultb_list_get_items,
		.get_list_item = NULL,
		.get_hint = xultb_list_get_hint,
		.set_action_listener = xultb_list_set_action_listener,
		.set_selected_index = xultb_list_set_selected_index,
		.oppcb = OPP_CB_FUNC(xultb_list),
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
	switch(callback) {
	case OPPN_ACTION_INITIALIZE:
		memset(list, 0, sizeof(struct xultb_list));
		GUI_LOG("Creating list ..\n");
		{
			va_list apa;
			if(opp_super_cb(xultb_window)(&list->super_data, OPPN_ACTION_INITIALIZE, NULL, apa, 0)) {
				va_end(apa);
				return -1;
			}
			va_end(apa);
		}
		list->super_data.vtable = &vtable_xultb_window_list;
		opp_vtable_set(list, xultb_list);
		opp_indexed_list_create2(&list->_items, 4);
		list->vpos = 1;
		list->item_font = xultb_font_create();
		if(cb_data) {
			GUI_LOG("Setting title ..\n");
			xultb_str_t*title = cb_data;
			if(title)list->super_data.title = title;
			xultb_str_t*default_command = va_arg(ap, xultb_str_t*);
			if(default_command)list->default_command = OPPREF(default_command);
		}
		GUI_LOG("Created xultb_list\n");
		return 0;
	case OPPN_ACTION_FINALIZE:
		OPPUNREF(list->default_command);
		opp_factory_destroy(&list->_items);
		{
			va_list ap;
			if(vtable_xultb_window.oppcb(&list->super_data, OPPN_ACTION_FINALIZE, NULL, ap, 0)) {
				return -1;
			}
		}
		break;
	}
	return 0;
}


static struct opp_factory xultb_list_factory;
struct xultb_list*xultb_list_create(xultb_str_t*title, xultb_str_t*default_command) {
	return opp_alloc4(&xultb_list_factory, 0, 0, title, default_command);
}

int xultb_list_system_init() {
	vtable_xultb_window_list = vtable_xultb_window;
	vtable_xultb_window_list.paint = xultb_list_window_paint_wrapper;
	vtable_xultb_window_list.handle_event = xultb_list_window_handle_event_wrapper;
	SYNC_ASSERT(OPP_FACTORY_CREATE(
			&xultb_list_factory
			, 1,sizeof(struct xultb_list)
			, OPP_CB_FUNC(xultb_list)) == 0);
	return 0;
}

C_CAPSULE_END
