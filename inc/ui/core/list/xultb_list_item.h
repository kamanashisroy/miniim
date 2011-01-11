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

#include "config"
#include "core/xultb_exttypes.h"
#include "ui/core/xultb_img.h"

#define LIST_ITEM_SIGNATURE 0x93
struct xultb_list_item {
	int sgn1;
	struct font ITEM_FONT;
	int FONT_HEIGHT;
	int PADDING;
	int DPADDING;
	int (*paint)(struct graphics g, int x, int y, int width, int selected);
	int sgn2;
	void *__more__;
	int sgn3;
};

#define CHECK_LIST_ITEM(x) assert(x->sgn1 == LIST_ITEM_SIGNATURE && x->sgn2 == LIST_ITEM_SIGNATURE && x->sgn3 == LIST_ITEM_SIGNATURE)


struct list_item*create_label(xultb_str_t label, xultb_img_t*img);
struct list_item*create_label(xultb_str_t label, xultb_img_t*img, xultb_bool_t change_bg_on_focus, xultb_bool_t truncate_text_to_fit_width);
struct list_item*create_selection_box(xultb_str_t label, xultb_str_t text, xultb_bool_t editable);
struct list_item*create_text_input(xultb_str_t label, xultb_str_t text, xultb_bool_t wrapped, xultb_bool_t editable);
struct list_item*create_text_input(xultb_str_t label, xultb_str_t text);
struct list_item*create_checkbox(xultb_str_t label, xultb_bool_t checked, xultb_bool_t editable);
struct list_item*create_radio_button(xultb_str_t label, xultb_bool_t checked, xultb_bool_t editable);
struct list_item*create_checkbox(xultb_str_t label, xultb_bool_t checked, xultb_bool_t editable, xultb_bool_t isRadio);



