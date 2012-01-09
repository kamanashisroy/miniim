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

#include "opp/opp_salt.h"
#include "opp/opp_any_obj.h"
#include "core/logger.h"
#include "ui/core/xultb_font.h"
#include "ui/core/xultb_text_format.h"
#include "ui/page/xultb_markup_item.h"

#if 0
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
#endif

opp_class_declare_novtable(xultb_markup_item,
	opp_class_extend(struct xultb_list_item);
//	struct xultb_ml_node*root; // Document root
	/**
	 * y-coordinate position of the image
	 */
	int xPos; // = 0
	int yPos;// = 0;

//	struct xultb_graphics*g;// = null;

	int lineHeight; // = 0;
	int width;
	xultb_bool_t selected;// = false;
	struct xultb_media_loader*loader;// = null;
);

static int minLineHeight = 0;// = Font.getFont(Font.FACE_SYSTEM, Font.STYLE_PLAIN,Font.SIZE_SMALL).getHeight()+PADDING;

static void clear_line_full(struct xultb_markup_item*item, struct xultb_graphics*g, int y, int height) {
	if (!item->selected)
		return;
	int oldColor = g->get_color(g);
	// #expand g->set_color(%net.ayaslive.miniim.ui.core.markup.bgHover%);
	g->set_color(g, 0xCCCCCC);
	g->fill_rect(g, 0, y, item->width, height);
	g->set_color(g, oldColor);
}

static void clear_line(struct xultb_markup_item*item, struct xultb_graphics*g) {
	if (!item->selected)
		return;
	clear_line_full(item, g, item->yPos, item->lineHeight);
}

static void break_line(struct xultb_markup_item*item, struct xultb_graphics*g) {
	// put a line break
	item->yPos += item->lineHeight;
	item->xPos = 0;

	// reset line height to minimum
	item->lineHeight = minLineHeight;

	// clear the next line
	clear_line(item, g);
}

static void update_height(struct xultb_markup_item*item, struct xultb_graphics*g, int newHeight) {
	if (newHeight > item->lineHeight) {
		// fill with background color
		clear_line_full(item, g, item->yPos + item->lineHeight, newHeight - item->lineHeight);
	}
	item->lineHeight = newHeight;
}

static void update_height_for_font(struct xultb_markup_item*item, struct xultb_graphics*g, xultb_font_t*font) {
	if (!font) {
		return;
	}
	const int height = font->get_height(font);
	if (item->lineHeight < (height + XULTB_LIST_ITEM_PADDING)) {
		update_height(item, g, height + XULTB_LIST_ITEM_PADDING);
	}
}

static xultb_bool_t is_focused(struct xultb_ml_node*elem) {
	xultb_str_t*focused = xultb_ml_get_attribute_value(elem, "focused");
	if(focused && xultb_str_equals_static(focused, "yes")) {
		return XULTB_TRUE;
	}
	return XULTB_FALSE;
}

static int is_active(struct xultb_ml_node*elem) {
	xultb_str_t*active = xultb_ml_get_attribute_value(elem, "active");
	if(active && xultb_str_equals_static(active, "yes")) {
		return 1;
	}
	return 0;
}

static void renderImage(struct xultb_markup_item*item, struct xultb_ml_node*elem) {
#if 0
	xultb_str_t* src = elem->get_attribute_value(null, "src");
	if (src == null) {
		return;
	}
	Image img = loader.getImage(src);
	if (img == null) {
		return;
	}
	int imgWidth = img.getWidth();
	int imgHeight = img.getHeight();

	// so we can use it inline
	xultb_str_t* position = elem->get_attribute_value(null, "p");
	if (position != null) {
		break_line();
		if (position.equals("c")) {
			position = null;
		}
		g.drawImage(img, (position == null) ? width / 2 : xPos, yPos,
				Graphics.TOP
						| ((position == null) ? Graphics.HCENTER
								: position.equals("l") ? Graphics.LEFT
										: Graphics.RIGHT));
		update_height(imgHeight + XULTB_LIST_ITEM_PADDING);
		break_line();
	} else {
		if (width - xPos < imgWidth) {
			break_line();
		}
		g.drawImage(img, xPos, yPos, Graphics.TOP | Graphics.LEFT);

		// increase line height if the image height is larger ..
		imgHeight += XULTB_LIST_ITEM_PADDING;
		update_height(imgHeight > lineHeight ? imgHeight : lineHeight);

		xPos += imgWidth;
		xPos += 4;/* finally add a space: 4px */
		if (width - xPos < 0) { /* pushed too much */
			break_line();
		}
	}
#endif
}

static void render_text(struct xultb_markup_item*item, struct xultb_graphics*g, xultb_font_t*font, xultb_str_t* text) {
	int off, ret;
//	text = text.replace('\n', ' ').replace('\r', ' ').trim(); /*< skip the newlines */
	if (text->len == 0) { /*< empty xultb_str_t* .. skip */
		return;
	}
	g->set_font(g, font);
	update_height_for_font(item, g, font);

	off = 0;
	while ((ret = xultb_wrap_next(text, font, off, item->width - item->xPos)) != -1) {
		xultb_str_t subtext;
		// draw the texts ..
		if (ret > off) {
			// draw the line
			xultb_substring(text, off, ret, &subtext);
			g->draw_string(g, &subtext, item->xPos, item->yPos, 1000, 1000, XULTB_GRAPHICS_TOP
					| XULTB_GRAPHICS_LEFT);
			item->xPos += font->substring_width(font, text, off, ret - off);
		}
		if (ret == off /* no place to write a word .. */
		|| ret < text->len /* there are more words so that we span into new line .. */
		|| item->width - item->xPos < 0 /* pushed too much */
		) {
			break_line(item, g);
		}
		off = ret;
	}

	if (item->xPos != 0) {
		item->xPos += 4;/* finally add a space: 4px */
		if (item->width - item->xPos < 0) { /* pushed too much */
			break_line(item, g);
		}
	}
}

static void render_node(struct xultb_markup_item*item, struct xultb_graphics*g
		, struct xultb_font*font, struct xultb_ml_node*elem) {
	xultb_str_t* tagName = elem->vtable->get_name(elem); /* Element name */
	struct xultb_font*newFont = font;
	int oldColor = g->get_color(g);
	SYNC_ASSERT(tagName->len != 0);

	if (xultb_str_equals_static(tagName, "br")) {
		break_line(item, g);
	} else if (xultb_str_equals_static(tagName, "img")) {
		renderImage(item, elem);
	} else if (xultb_str_equals_static(tagName, "b")) {
		newFont = xultb_font_get(xultb_font_get_face(font), xultb_font_get_style(font)
				| XULTB_FONT_STYLE_BOLD, xultb_font_get_size(font));
	} else if (xultb_str_equals_static(tagName, "i")) {
		newFont = xultb_font_get(xultb_font_get_face(font), xultb_font_get_style(font)
				| XULTB_FONT_STYLE_ITALIC, xultb_font_get_size(font));
	} else if (xultb_str_equals_static(tagName, "big")) {
		newFont
				= xultb_font_get(xultb_font_get_face(font), xultb_font_get_style(font), XULTB_FONT_SIZE_LARGE);
	} else if (xultb_str_equals_static(tagName, "small")) {
		newFont
				= xultb_font_get(xultb_font_get_face(font), xultb_font_get_style(font), XULTB_FONT_SIZE_SMALL);
	} else if (xultb_str_equals_static(tagName, "strong") || xultb_str_equals_static(tagName, "em")) {
		/// \xxx what to do for strong text ??
		newFont = xultb_font_get(xultb_font_get_face(font), xultb_font_get_style(font)
				| XULTB_FONT_STYLE_BOLD, XULTB_FONT_SIZE_MEDIUM);
	} else if (xultb_str_equals_static(tagName, "u")) {
		newFont = xultb_font_get(xultb_font_get_face(font), xultb_font_get_style(font)
				| XULTB_FONT_STYLE_UNDERLINED, xultb_font_get_size(font));
	} else if (xultb_str_equals_static(tagName, "p")) {
		// line break
		break_line(item, g);
		break_line(item, g);
	} else if (xultb_str_equals_static(tagName, "a")) {

		xultb_str_t* link = xultb_ml_get_attribute_value(elem, "href");

		// draw the anchor
		if (!OPP_FACTORY_USE_COUNT(&elem->children) || !link) {
			// skip empty links
		} else if (is_focused(elem)) {
			// #expand g->set_color(%net.ayaslive.miniim.ui.core.markup.aFgHover%);
			g->set_color(g, 0x0000FF);
			// #expand newFont = xultb_font_get(xultb_font_get_face(font), xultb_font_get_style(font) | %net.ayaslive.miniim.ui.core.markup.aFontHover%, xultb_font_get_size(font));
			newFont = xultb_font_get(xultb_font_get_face(font), xultb_font_get_style(font)
					| XULTB_FONT_STYLE_UNDERLINED | XULTB_FONT_STYLE_BOLD, xultb_font_get_size(font));
			// SimpleLogger.debug(this, "renderNode()\t\tFocused:" + elem->getChild(0));
		} else if (is_active(elem)) {
			// #expand g->set_color(%net.ayaslive.miniim.ui.core.markup.aFgActive%);
			g->set_color(g, 0xCC99FF);

			// #expand newFont = xultb_font_get(xultb_font_get_face(font), xultb_font_get_style(font) | %net.ayaslive.miniim.ui.core.markup.aFontActive%, xultb_font_get_size(font));
			newFont = xultb_font_get(xultb_font_get_face(font), xultb_font_get_style(font)
					| XULTB_FONT_STYLE_UNDERLINED, xultb_font_get_size(font));
			// SimpleLogger.debug(this, "renderNode()\t\tActive:" + elem->getChild(0));
		} else {
			// #expand g->set_color(%net.ayaslive.miniim.ui.core.markup.aFg%);
			g->set_color(g, 0x0000FF);

			// #expand newFont = xultb_font_get(xultb_font_get_face(font), xultb_font_get_style(font) | %net.ayaslive.miniim.ui.core.markup.aFont%, xultb_font_get_size(font));
			newFont = xultb_font_get(xultb_font_get_face(font), xultb_font_get_style(font)
					| XULTB_FONT_STYLE_UNDERLINED, xultb_font_get_size(font));
		}
	} else {
		// We do not know how to handle this element
		// SimpleLogger.debug(this, "renderNode()\t\tNothing to do for: " + tagName);
		// go on with inner elements
	}
	// render the inner nodes
	// System.out.println("<"+tagName+">");
//	int count = OPP_FACTORY_USE_COUNT(elem->children);//elem->getChildCount(elem);
	int i;
	for (i = 0;; i++) {
		struct xultb_ml_elem*obj;
		opp_at_ncode(obj, &elem->children, i,
			switch (obj->type) {
			case XULTB_ELEMENT_TEXT:
				render_text(item, g, newFont, obj->content);
				break;
			case XULTB_ELEMENT_NODE:
				render_node(item, g, newFont, (struct xultb_ml_node*) obj);
				break;
			default:
				SYNC_LOG(SYNC_VERB, "Nothing to do for %s\n", obj->content->str);
				break;
			}
		) else {
			break;
		}
	}
	// System.out.println("</"+tagName+">");
	g->set_color(g, oldColor);
}

static int getActualCardHeight(struct xultb_markup_item*item) {
	return item->yPos;
}

/** List item implementation .. */
/*@{*/
static int paint_impl(struct xultb_list_item*li, struct xultb_graphics*g, int x, int y, int width, int is_selected) {
	struct xultb_markup_item*item = (struct xultb_markup_item*)li;
	item->xPos = 0;
	item->yPos = 0;
	item->width = width;
//	item->is_selected = is_selected;
	// #expand g->set_color(%net.ayaslive.miniim.ui.core.markup.fg%);
	g->set_color(g, 0x006699);
	item->lineHeight = minLineHeight;
//	g.translate(x, y);

	// draw the line background
	clear_line(item, g);

	struct xultb_font*font = xultb_font_get(XULTB_FONT_FACE_DEFAULT, XULTB_FONT_STYLE_PLAIN, XULTB_FONT_SIZE_SMALL);
	// draw the node recursively
	render_node(item, g, font, (struct xultb_ml_node*)item->super_data.target);


	int ret = item->yPos;
	if (item->xPos != 0) {
		ret = item->yPos + item->lineHeight;
	}

	// fix the position and font
//	g->translate(-x, -y);
//	g->set_font(ITEM_FONT);
	return ret;
}

struct opp_vtable_xultb_list_item vtable_xultb_list_item_overriden;
opp_vtable_extern(xultb_list_item);
OPP_CB(xultb_markup_item) {
	struct xultb_markup_item*item = (struct xultb_markup_item*)data;
	memset(item, 0, sizeof(*item));
	switch(callback) {
	case OPPN_ACTION_INITIALIZE:
		item->loader = va_arg(ap, struct xultb_media_loader*);
		{
			va_list dummy;
			opp_super_cb(xultb_list_item)(&item->super_data, OPPN_ACTION_INITIALIZE, NULL, dummy, 0);
		}
		OPPUNREF(item->super_data.target); // may be not needed
		item->super_data.target = OPPREF(cb_data);
		opp_vtable_set(&item->super_data, xultb_list_item_overriden);
		return 0;
	case OPPN_ACTION_FINALIZE:
		{
			va_list dummy;
			opp_super_cb(xultb_list_item)(&item->super_data, OPPN_ACTION_FINALIZE, NULL, dummy, 0);
		}
		break;
	}
	return 0;
}

static struct opp_factory markup_item_factory;
struct xultb_list_item*xultb_markup_item_create(struct xultb_ml_node*root
		, struct xultb_media_loader*loader, xultb_bool_t selectable
		, struct xultb_event_listener*el) {
	return opp_alloc4(&markup_item_factory
				, 0, 0, root, loader, selectable, el);
}

int xultb_markup_item_system_init() {
	vtable_xultb_list_item_overriden = vtable_xultb_list_item;
	vtable_xultb_list_item_overriden.paint = paint_impl;
	struct xultb_font*font = xultb_font_get(XULTB_FONT_FACE_SYSTEM, XULTB_FONT_STYLE_PLAIN, XULTB_FONT_SIZE_SMALL);
	minLineHeight = font->get_height(font) + XULTB_LIST_ITEM_PADDING;
	OPPUNREF(font);
	SYNC_ASSERT(!OPP_FACTORY_CREATE(
			&markup_item_factory
			, 1,sizeof(struct xultb_markup_item)
			, OPP_CB_FUNC(xultb_markup_item)));
	return 0;
}

int xultb_markup_item_system_deinit() {
	opp_factory_destroy(&markup_item_factory);
	return 0;
}
