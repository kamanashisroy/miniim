#ifndef XULTB_LIST_ITEM_H
#define XULTB_LIST_ITEM_H

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

#include "core/xultb_exttypes.h"
#include "ui/core/xultb_img.h"
#include "ui/core/xultb_graphics.h"

C_CAPSULE_START

enum xultb_list_item_type {
	XULTB_LIST_ITEM_LABEL,
	XULTB_LIST_ITEM_SELECTION,
	XULTB_LIST_ITEM_TEXT_INPUT,
	XULTB_LIST_ITEM_CHECKBOX,
};


enum {
	XULTB_LIST_ITEM_PADDING = 2,
	XULTB_LIST_ITEM_RESOLUTION = 3,
	XULTB_LIST_ITEM_DPADDING = 5,
};


opp_vtable_declare(xultb_list_item,
	struct xultb_font*ITEM_FONT;
	int FONT_HEIGHT;
	int (*paint)(struct xultb_list_item*item, struct xultb_graphics*g, int x, int y, int width, int selected);
);


#define LIST_ITEM_SIGNATURE 0x93
opp_class_declare(xultb_list_item,
	xultb_str_t label;
	xultb_str_t text;
	xultb_bool_t checked;
	xultb_bool_t is_editable;
	xultb_bool_t is_radio;
	xultb_bool_t wrapped;
	xultb_bool_t truncate_text_to_fit_width;
	xultb_bool_t focused;
	xultb_img_t*img;
	enum xultb_list_item_type type;
	void *__more__;
);

struct xultb_list_item*xultb_list_item_create_label(xultb_str_t*label, xultb_img_t*img);
struct xultb_list_item*xultb_list_item_create_label_full(xultb_str_t*label, xultb_img_t*img, xultb_bool_t change_bg_on_focus, xultb_bool_t truncate_text_to_fit_width);
struct xultb_list_item*xultb_list_item_create_selection_box(xultb_str_t*label, xultb_str_t*text, xultb_bool_t editable);
struct xultb_list_item*xultb_list_item_create_text_input_full(xultb_str_t*label, xultb_str_t*text, xultb_bool_t wrapped, xultb_bool_t editable);
struct xultb_list_item*xultb_list_item_create_text_input(xultb_str_t*label, xultb_str_t*text);
struct xultb_list_item*xultb_list_item_create_checkbox(xultb_str_t*label, xultb_bool_t checked, xultb_bool_t editable);
struct xultb_list_item*xultb_list_item_create_radio_button(xultb_str_t*label, xultb_bool_t checked, xultb_bool_t editable);

int xultb_list_item_system_init();

C_CAPSULE_END

#endif // XULTB_LIST_ITEM_H
