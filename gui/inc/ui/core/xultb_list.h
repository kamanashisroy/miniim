#ifndef XULTB_LIST_H
#define XULTB_LIST_H

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

#include "opp/opp_type.h"
#include "core/config.h"
#include "ui/xultb_guicore.h"

C_CAPSULE_START

opp_vtable_declare(xultb_list,
	void (*set_action_listener)(struct xultb_list*list, struct xultb_action_listener*lis);

	struct opp_factory*(*get_items)(struct xultb_list*list);
	struct xultb_list_item* (*get_list_item)(struct xultb_list*list, void*data);
	int (*get_count)(struct xultb_list*list);
	xultb_str_t*(*get_hint)(struct xultb_list*list);

	void*(*get_selected)(struct xultb_list*list);

	void (*set_selected_index)(struct xultb_list*list, int index);
	int (*get_selected_index)(struct xultb_list*list);

	xultb_bool_t (*handle_item)(struct xultb_list*list, void*target, int flags, int key_code, int x, int y);
);

opp_class_declare(xultb_list,
	opp_class_extend(struct xultb_window);

	xultb_font_t*item_font;
	xultb_bool_t continuous_scrolling;
	
	int vpos; /* Index of the showing Item */
	int selected_index;
	
	int leftMargin;
	int topMargin;
	int rightMargin;
	int bottomMargin;
	int RESOLUTION;

	xultb_str_t*default_command;
//	struct xultb_action_listener*lis;
	struct opp_factory _items;
);


struct xultb_list*xultb_list_create(xultb_str_t*title, xultb_str_t*default_command);
int xultb_list_system_init();

C_CAPSULE_END

#endif // XULTB_LIST_H
