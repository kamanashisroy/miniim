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
/** \todo support object item showing, truncated text showing .. */
struct xultb_list {
	/*! \todo show arrow signs(left and right arrow) to indicate that the text is truncated */
	int HMARGIN;
	int VMARGIN;
	
	xultb_bool_t continuous_scrolling;
	
	int vpos; /* Index of the showing Item */
	int selected_index;
	
	int leftMargin;
	int topMargin;
	int rightMargin;
	int bottomMargin;
	int RESOLUTION;

	xultb_str_t default_command;
	
	void (*set_action_listener)(struct xultb_action_listener lis);
	
	struct obj_factory*(*get_items)();
	struct xmltb_list_item* get_list_item(void*data);
	xmltb_str_t*(*getHint)();
	
	struct xmltb_list_item get_selected();
	
	void (*set_selected_index)(int index);
	int get_selected_index();
	
	void paint(struct graphics*g);
}

create_list(xultb_str_t*title, xultb_str_t*default_command);
