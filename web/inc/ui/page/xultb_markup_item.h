#ifndef XULTB_MARKUP_LIST_ITEM_H
#define XULTB_MARKUP_LIST_ITEM_H


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
#include "pull_parser/xultb_parser_core.h"
#include "ui/xultb_guicore.h"
#include "ui/page/xultb_event_listener.h"

C_CAPSULE_START

opp_vtable_declare(xultb_markup_item,
//	int (*initialize)(struct xultb_markup_item*item, void*data);
//	void (*render_node)(struct xultb_markup_item*item, struct xultb_font*font, struct xultb_ml_elem*elem);
//	void (*render_text)(struct xultb_markup_item*item, struct xultb_font*font, const char*text);
//	void (*render_image)(struct xultb_markup_item*item, struct xultb_ml_elem*elem);
	void (*break_line)(struct xultb_markup_item*item);
	void (*clear_line)(struct xultb_markup_item*item);
	void (*clear_line_full)(struct xultb_markup_item*item, int y, int height);
	void (*update_height)(struct xultb_markup_item*item, struct xultb_font*font);
	void (*update_height_force)(struct xultb_markup_item*item, int newHeight);

	/** List item implementation .. */
	/*@{*/
//	int (*paint)(struct graphics g, int x, int y, int width, boolean selected);
	/*@}*/

	void (*free)();
	
	/// traverser
	/*@{*/
	void (*set_listener)(struct xultb_markup_item*item, struct xultb_event_listener*lis);
	
	void (*free_traverser)(struct xultb_markup_item*item);
	/** Key press listener to traverse the browsable elements */
	int (*key_pressed)(struct xultb_markup_item*item, int key_code, int game_action);
	void (*search_focusable_elements)(struct xultb_markup_item*item, struct xultb_ml_node*elem);
	void (*set_focus)(struct xultb_markup_item*item, struct xultb_ml_node*elem);
//	int (*is_focused)(struct xultb_markup_item*item, struct xultb_ml_elem*elem);
	int (*is_active)(struct xultb_markup_item*item, struct xultb_ml_node*elem);
	/*@}*/
);

opp_class_declare(xultb_markup_item,
	opp_class_extend(struct xultb_list_item);
	struct xultb_ml_node*root; // Document root
	/**
	 * y-coordinate position of the image
	 */
	int xPos; // = 0
	int yPos;// = 0;

//	struct xultb_graphics*g;// = null;

	int lineHeight; // = 0;
	int width;
	int minLineHeight;// = Font.getFont(Font.FACE_SYSTEM, Font.STYLE_PLAIN,Font.SIZE_SMALL).getHeight()+PADDING;
	xultb_bool_t selected;// = false;
	struct xultb_media_loader*loader;// = null;
	struct xultb_markup_item*single_instance;// = null;
);

struct markup_item*xultb_markup_item_create(struct xultb_ml_node*root
		, struct xultb_media_loader*loader, xultb_bool_t selectable
		, struct xultb_event_listener*el);


C_CAPSULE_END

#endif // XULTB_MARKUP_ITEM_H
