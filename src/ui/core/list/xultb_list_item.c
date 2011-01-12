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
#include "ui/core/xultb_text_format.h"
#include "ui/core/list/xultb_list_item.h"


struct xultb_list_item*create_label(xultb_str_t*label, xultb_img_t*img) {
	return create_label_full(label, img, XULTB_TRUE, XULTB_FALSE);
}

struct xultb_list_item*create_label_full(xultb_str_t*label, xultb_img_t*img, xultb_bool_t change_bg_on_focus, xultb_bool_t truncate_text_to_fit_width) {
	struct xultb_list_item*item = obj_factory_alloc(label_factory);
	item->label = *label;
	item->img = img;
	item->is_editable = change_bg_on_focus;
	item->type = XULTB_LIST_ITEM_LABEL;
	item->truncate_text_to_fit_width = truncate_text_to_fit_width;
	return item;
}

struct xultb_list_item*create_selection_box(xultb_str_t*label, xultb_str_t*text, xultb_bool_t editable) {
	struct xultb_list_item*item = obj_factory_alloc(label_factory);
	item->label = *label;
	item->text = text;
	item->is_editable = editable;
	item->type = XULTB_LIST_ITEM_SELECTION;
	return item;
}

struct xultb_list_item*create_text_input_full(xultb_str_t*label, xultb_str_t*text, xultb_bool_t wrapped, xultb_bool_t editable) {
	struct xultb_list_item*item = obj_factory_alloc(label_factory);
	item->label = *label;
	item->text = (text == NULL)?xultb_str_create(""):*text;
	item->wrapped = wrapped;
	item->is_editable = editable;
	item->type = XULTB_LIST_ITEM_TEXT_INPUT;
	return item;
}

struct xultb_list_item*create_text_input(xultb_str_t*label, xultb_str_t*text) {
	return create_text_input_full(label, text, XULTB_FALSE, XULTB_TRUE);
}

static struct xultb_list_item*create_checkbox_full(xultb_str_t*label, xultb_bool_t checked, xultb_bool_t editable, xultb_bool_t is_radio) {
	struct xultb_list_item*item = obj_factory_alloc(label_factory);
	item->label = *label;
	item->checked = checked;
	item->is_editable = editable;
	item->type = XULTB_LIST_ITEM_CHECKBOX;
	item->img = NULL;
	item->is_radio = is_radio;
	return item;
}

struct xultb_list_item*create_checkbox(xultb_str_t*label, xultb_bool_t checked, xultb_bool_t editable) {
	return create_checkbox_full(label, checked, editable, XULTB_FALSE);
}

struct xultb_list_item*create_radio_button(xultb_str_t*label, xultb_bool_t checked, xultb_bool_t editable) {
	return create_checkbox_full(label, checked, editable, XULTB_TRUE);
}

enum {
	XULTB_LIST_ITEM_PADDING = 2,
	XULTB_LIST_ITEM_RESOLUTION = 3,
	XULTB_LIST_ITEM_DPADDING = 5,
};

static void draw_selectionbox_icon(struct xultb_graphics*g, int x, int y,
		xultb_bool_t focused) {
	// now indicate that it is checked ..
	/* draw a rectangle */
	// #expand g.setColor(focused?(isEditable?%net.ayaslive.miniim.ui.core.list.listitemfactory.boxHover%:%net.ayaslive.miniim.ui.core.list.listitemfactory.boxHoverInactive%):%net.ayaslive.miniim.ui.core.list.listitemfactory.box%);
	g->set_color(g, focused ? (isEditable ? 0x006699 : 0x999999) : 0xCCCCCC);
	g->fill_triangle(g, x + RESOLUTION / 2, y + RESOLUTION, x + RESOLUTION, y, x, y);
}


static void draw_checkbox_icon(struct xultb_graphics*g, int x, int y,
		xultb_bool_t focused) {
	// draw a box
	// #expand g.setColor(focused?(isEditable?%net.ayaslive.miniim.ui.core.list.listitemfactory.boxHover%:%net.ayaslive.miniim.ui.core.list.listitemfactory.boxHoverInactive%):%net.ayaslive.miniim.ui.core.list.listitemfactory.box%);
	g->set_color(focused ? (isEditable ? 0x006699 : 0x999999) : 0xCCCCCC);
	if (isRadio) {

		// make the box a little curcular as it is radio button ..
		g->draw_round_rect(g, x + 1, y + 1, RESOLUTION - 1, RESOLUTION - 1, DPADDING,
				DPADDING);
	} else {

		g->draw_rect(g, x + 1, y + 1, RESOLUTION - 1, RESOLUTION - 1);
	}

	// now indicate that it is checked ..
	if (checked) {

		if (isRadio) {

			g->fill_round_rect(g, x + 2, y + 2, RESOLUTION - 2, RESOLUTION - 2,
					DPADDING, DPADDING);
		} else {

			// #ifdef net.ayaslive.miniim.ui.core.list.listitemfactory.useTick
			//@     // draw tick
			//@     int bendX = x + RESOLUTION/4 + 1;
			//@     g.drawLine(x + 1, y + RESOLUTION/2, bendX, y + RESOLUTION - 1);
			//@     g.drawLine(bendX, y + RESOLUTION - 2, x + RESOLUTION - 1, y + 1);
			// #else
			g->fill_rect(g, x + 2, y + 2, RESOLUTION - 2, RESOLUTION - 2);
			// #endif
		}
	}
}


static int xultb_list_item_paint(struct xultb_list_item*item, struct xultb_graphics*g, int x, int y, int width, int selected) {
	int start, pos, ret, labelWidth, labelHeight, lineCount;
	int imgspacing = 0;
	if (item->img != NULL) {
		imgspacing = item->img->width + XULTB_LIST_ITEM_PADDING;
	}
	if (item->type == XULTB_LIST_ITEM_TEXT_CHECKBOX) {
		imgspacing = XULTB_LIST_ITEM_RESOLUTION + XULTB_LIST_ITEM_PADDING;
	}

	// Write the Label
	labelWidth = labelHeight = start = pos = ret = lineCount = 0;
	if (item->label.len != 0) {

		// #expand g.setColor(%net.ayaslive.miniim.ui.core.list.listitemfactory.fg%);
		g->set_color(g, 0x000000);
		while ((pos = xultb_wrap_next(&item->label, item->ITEM_FONT, start, width
				- imgspacing - XULTB_LIST_ITEM_DPADDING)) != -1) {
			if (item->focused && item->is_editable && item->type == LABEL) {
				// #expand g.setColor(%net.ayaslive.miniim.ui.core.list.listitemfactory.bgHover%);
				g->set_color(g, 0x0099CC);

				// draw the advanced background
				// #ifdef net.ayaslive.miniim.ui.core.list.listitemfactory.bgShadeHover

				// Do special things if we are in the first line
				if (start == 0) {

					// draw rounded background
					// #expand g.fillRoundRect( x, y + ret, width, FONT_HEIGHT + XULTB_LIST_ITEM_DPADDING, %net.ayaslive.miniim.ui.core.rounded_corner_radius%, %net.ayaslive.miniim.ui.core.rounded_corner_radius%);
					g->fill_round_rect(x, y + ret, width, FONT_HEIGHT + XULTB_LIST_ITEM_DPADDING,
							4, 4);

					// draw the shadow
					// #expand g.setColor(%net.ayaslive.miniim.ui.core.list.listitemfactory.bgShadeHover%);
					g->set_color(g, 0x00CCFF);

					// #expand g.fillRect( x + %net.ayaslive.miniim.ui.core.rounded_corner_radius%/2, y + ret + %net.ayaslive.miniim.ui.core.rounded_corner_radius%/2, width - %net.ayaslive.miniim.ui.core.rounded_corner_radius%, FONT_HEIGHT/2 + XULTB_LIST_ITEM_PADDING - %net.ayaslive.miniim.ui.core.rounded_corner_radius%);
					g->fillRect(g, x + 4 / 2, y + ret + 4 / 2, width - 4,
							FONT_HEIGHT / 2 + XULTB_LIST_ITEM_PADDING - 4);
				} else {
					g->fillRect(g, x, y + ret, width, FONT_HEIGHT + XULTB_LIST_ITEM_DPADDING);
				}

				// #else
				//@
				//@         // draw plain background
				//@         g.fillRect( x, y + ret, width, FONT_HEIGHT + XULTB_LIST_ITEM_DPADDING);
				// #endif

				// draw text
				// #expand g.setColor(%net.ayaslive.miniim.ui.core.list.listitemfactory.fgHover%);
				g->set_color(g, 0xFFFFFF);
			}
			g->draw_string(g, xultb_substring(&item->label, start, pos), x + imgspacing + XULTB_LIST_ITEM_PADDING,
					y + ret + XULTB_LIST_ITEM_PADDING, XULTB_GRAPHICS_TOP | XULTB_GRAPHICS_LEFT);
			ret += item->FONT_HEIGHT + XULTB_LIST_ITEM_DPADDING;
			start = pos;
			if (start == 0) {
				imgspacing = 0;
			}
			if (item->truncate_text_to_fit_width) {
				break;
			}
		}
		if (item->type != XULTB_LIST_ITEM_LABEL && item->type != XULTB_LIST_ITEM_TEXT_CHECKBOX) {
			labelWidth = item->ITEM_FONT->string_width(&item->label);
			if (!item->wrapped && (labelWidth) > width - XULTB_LIST_ITEM_DPADDING) {
				item->wrapped = false;
			}
			labelHeight = ret;
			labelWidth += XULTB_LIST_ITEM_DPADDING;
		}
	}
	if (item->text.len != 0) {

		if (item->wrapped) {

			lineCount = 0;
			if (item->text.len != 0) { /* when the string length is Zero */
				/* write the text in the next line */
				start = pos = 0;
				while ((pos = xultb_wrap_next(&item->text, item->ITEM_FONT, start, width
						- XULTB_LIST_ITEM_DPADDING)) != -1 && lineCount < 3) {
					g->draw_string(g, xultb_substring(&item->text, start, pos), x + XULTB_LIST_ITEM_PADDING, y
							+ ret + XULTB_LIST_ITEM_PADDING, Graphics.TOP | Graphics.LEFT);
					ret += FONT_HEIGHT + XULTB_LIST_ITEM_DPADDING;
					start = pos;
					lineCount++;
				}
			}
			/* we are trying to show 3 lines always, not more not less */
			ret += (3 - lineCount) * (FONT_HEIGHT + XULTB_LIST_ITEM_DPADDING);
			labelWidth = 0;
		} else {

			int imgWidth = 0;
			if (item->type == XULTB_LIST_ITEM_SELECTION) {
				imgWidth = XULTB_LIST_ITEM_RESOLUTION;
			}

			pos = xultb_wrap_next(&item->text, item->ITEM_FONT, 0, width - labelWidth
					- XULTB_LIST_ITEM_DPADDING - imgWidth - XULTB_LIST_ITEM_DPADDING);
			if (pos != -1) {
				g->draw_string(g, xultb_substring(item->text,0, pos), x + labelWidth + XULTB_LIST_ITEM_PADDING,
						y + XULTB_LIST_ITEM_PADDING, Graphics.TOP | Graphics.LEFT);
				if (pos < item->text.len) {
					/* show an image at last to indicate that there are more data .. */
				}
			}
			if (item->type == XULTB_LIST_ITEM_SELECTION) {
				drawSelectionBoxIcon(g, x + width - XULTB_LIST_ITEM_RESOLUTION - XULTB_LIST_ITEM_PADDING, y
						+ XULTB_LIST_ITEM_PADDING, focused);
			}
			labelHeight = 0;

		}
	}
	// Draw the image
	if (item->img != null) {
		g->draw_image(g, item->img, x + XULTB_LIST_ITEM_PADDING, y + XULTB_LIST_ITEM_PADDING, Graphics.TOP | Graphics.LEFT);
	}

	if (item->type == XULTB_LIST_ITEM_TEXT_CHECKBOX) {
		drawCheckBoxIcon(g, x + XULTB_LIST_ITEM_PADDING, y + XULTB_LIST_ITEM_PADDING, focused);
	}

	// #expand g.setColor(focused?(isEditable?%net.ayaslive.miniim.ui.core.list.listitemfactory.boxHover%:%net.ayaslive.miniim.ui.core.list.listitemfactory.boxHoverInactive%):%net.ayaslive.miniim.ui.core.list.listitemfactory.box%);
	g->set_color(g, item->focused ? (item->is_editable ? 0x006699 : 0x999999) : 0xCCCCCC);

	/* draw a square */
	if (item->type != XULTB_LIST_ITEM_TEXT_CHECKBOX && (item->focused || item->type != XULTB_LIST_ITEM_LABEL)
	// #ifdef net.ayaslive.miniim.ui.core.list.listitemfactory.bgShadeHover
			&& !(item->focused && item->type == XULTB_LIST_ITEM_LABEL)
	// #endif
	) {
		//      #ifdef net.ayaslive.miniim.ui.core.rounded_corner_radius
		//      #expand g.drawRoundRect( x + labelWidth, y + labelHeight, width - labelWidth, ret - labelHeight, %net.ayaslive.miniim.ui.core.rounded_corner_radius%, %net.ayaslive.miniim.ui.core.rounded_corner_radius%);
		g->drawRoundRect(g, x + labelWidth, y + labelHeight, width - labelWidth,
				ret - labelHeight, 4, 4);
		//      #else
		//@     g.drawRect( x + labelWidth, y + labelHeight, width - labelWidth, ret - labelHeight);
		//      #endif
	}
	return ret;

}

static int xultb_list_item_initialize(void*data) {
	struct xultb_list_item*item = data;
	item->paint = xultb_list_item_paint;
}

#if 0
static int xultb_list_item_finalize(void*data) {
	// TODO fill me
}
#endif

