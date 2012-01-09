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

#include "core/logger.h"
#include "opp/opp_indexed_list.h"
#include "opp/opp_salt.h"
#include "ui/core/list/xultb_list_item.h"
#include "ui/page/xultb_markup_item.h"
#include "ui/page/xultb_page.h"
#include "ui/page/xultb_web_controler.h"
#include "ui/xultb_gui_input.h"

opp_vtable_extern(xultb_list);
extern xultb_str_t*DOT;
extern xultb_str_t*BLANK_STRING;
extern xultb_str_t*ASTERISKS_STRING;

static int update_impl(struct xultb_list_item*item, xultb_str_t*text) {
	struct xultb_ml_node*node = item->target;
	SYNC_ASSERT(node);
	xultb_str_t*new_text = OPPREF(text);
	OPPUNREF(node->elem.content);
	GUI_INPUT_LOG("setting new text %s\n", new_text->str);
	node->elem.content = new_text;
	return 0;
}

struct opp_vtable_xultb_list_item vtable_xultb_page_item;

static struct opp_factory* xultb_page_get_items(struct xultb_list*list) {
	struct xultb_page*mlist = (struct xultb_page*)list;
	SYNC_ASSERT(mlist->root);
	GUI_LOG("Obtaining items:%d\n", OPP_FACTORY_USE_COUNT(&mlist->root->children));
	return &mlist->root->children;
}

static void xultb_page_set_node(struct xultb_page*mlist
		, struct xultb_ml_node*node, int selectedIndex) {
	mlist->root = node;
//	searching = false;
	mlist->continuousScrolling = XULTB_TRUE;

	opp_extvt(mlist)->set_selected_index(&mlist->super_data, selectedIndex);

#if 0
	// see if the node is selection box ..
	if(node instanceof Element) {
		Element elem = (Element)node;
		if(elem.getName().equals("s")) {
			// see if it has multiple choice ..
			isMultipleSelection = DefaultComplexListener.isPositiveAttribute(elem, "m");
		}
	}
#endif
	mlist->super_data.super_data.vtable->show_full(&mlist->super_data.super_data, &mlist->left_menu, mlist->right_menu);
}

static struct xultb_list_item*xultb_page_get_item_helper(struct xultb_list*list, void*data) {
	struct xultb_page*mlist = (struct xultb_page*)list;
	struct xultb_ml_node*elem = (struct xultb_ml_node*)data;
	xultb_str_t*name = elem->vtable->get_name(elem);
	xultb_str_t*label = xultb_ml_get_attribute_value(elem, "l");
	if (!name || xultb_str_equals_static(name, "m")) {
		return (struct xultb_list_item*)xultb_markup_item_create(elem, mlist->ml, XULTB_FALSE, mlist->el);
	} else if (xultb_str_equals_static(name, "l")) {
		xultb_str_t*text = DOT;
#if 0
		if (OPP_FACTORY_USE_COUNT(&elem->children)) {
			void*obj;
			opp_at_ncode(obj, &elem->children, 0,
				text = ((struct xultb_ml_elem*)obj)->content;
				if (text) {
					text = xultb_str_trim(text);
				} else {
					text = DOT;
				}
			);
		}
#else
		text = xultb_ml_get_text(elem);
		text = xultb_str_trim(text);
#endif

		// see if the label has any image
		struct xultb_img*img = NULL;
		xultb_str_t*src = xultb_ml_get_attribute_value(elem, "src");
		if (src) {
			img = mlist->ml->get_image(src);
		}
		return xultb_list_item_create_label_full(text, img, xultb_ml_get_attribute_value(elem,
				"href") != NULL, XULTB_FALSE, elem);
	} else if (xultb_str_equals_static(name, "t")) {

		// get current text
		xultb_str_t*text = BLANK_STRING;
		if (/*!OPP_FACTORY_USE_COUNT(&elem->children) || */!(text = xultb_ml_get_text(elem))) {

			// get hint of this field
			text = xultb_ml_get_attribute_value(elem, "h");
			if (!text) {
				text = BLANK_STRING;
			}
		} else {
			// get rid of spaces
			text = xultb_str_trim(text);
		}

		// see if it is password field
		if (xultb_list_item_attr_is_positive(elem, "p")) {

			// in this case we hide the content of the password ..
			text = xultb_str_clone(ASTERISKS_STRING->str, text->len, 0);
		}

		// do not scroll continuously when there is textfield
		mlist->continuousScrolling = XULTB_FALSE;
		GUI_LOG("Text input box...... label:[%1.1s], text:[%1.1s]\n", label?label->str:"", text?text->str:"");

		struct xultb_list_item*ret = xultb_list_item_create_text_input_full(label, text,
				xultb_list_item_attr_is_positive(elem, "w"), XULTB_TRUE);
		return ret;
	} else if (xultb_str_equals_static(name, "s")) {

		// get selected index
		xultb_str_t*buffer = xultb_str_alloc(NULL, 512, NULL, 0);
		int first = XULTB_TRUE;
		int i;
//		int count = OPP_FACTORY_USE_COUNT(&elem->children);
		for (i = 0; ; i++) {
			struct xultb_ml_node*obj;
			opp_at_ncode2(obj, struct xultb_ml_node*, (&elem->children), i,
				// see if it is selected
				if (obj->elem.type == XULTB_ELEMENT_NODE && xultb_list_item_attr_is_positive(obj, "s")) {
					xultb_str_t*tmp = xultb_ml_get_text(obj);
					if (tmp) {
						if (first) {
							first = XULTB_FALSE;
						} else {
							xultb_str_cat_char(buffer, ',');
						}
						tmp = xultb_str_trim(tmp);
						xultb_str_cat(buffer, tmp);
					}
				}
			) else {
				break;
			}
		}

		// do not scroll continuously when there is selection box
		mlist->continuousScrolling = XULTB_FALSE;
		return xultb_list_item_create_selection_box(label, buffer,
				XULTB_TRUE);
	} else if (xultb_str_equals_static(name, "r")) {
		// render radio button

		return xultb_list_item_create_radio_button(label,
				xultb_list_item_attr_is_positive(elem, "c"), XULTB_TRUE);
	} else if (xultb_str_equals_static(name, "ch")) {
		// so it is checkbox

		return xultb_list_item_create_checkbox(label,
				xultb_list_item_attr_is_positive(elem, "c"), XULTB_TRUE);
	} else if (xultb_str_equals_static(name, "o")) {
		// so it is selection option
		// get current text
		xultb_str_t*text = NULL;
		if (OPP_FACTORY_USE_COUNT(&elem->children) == 0 || (text = xultb_ml_get_text(elem)) == NULL) {

			// get hint of this field
			text = xultb_ml_get_attribute_value(elem, "h");
			if (text == NULL) {
				text = BLANK_STRING;
			}
		} else {

			// get rid of spaces
			text = xultb_str_trim(text);
		}

		// see if it is multiple selection box ..
		if (mlist->isMultipleSelection) {

			// see if it is selected ..
			return xultb_list_item_create_checkbox(text,
					xultb_list_item_attr_is_positive(elem, "s"), XULTB_TRUE);
		} else {
			return xultb_list_item_create_label_full(text, NULL, XULTB_TRUE, XULTB_FALSE, NULL);
		}
	} else {
		return (struct xultb_list_item*)xultb_markup_item_create(elem, mlist->ml, XULTB_FALSE, mlist->el);
	}
	return NULL;
}

static struct xultb_list_item*xultb_page_get_item(struct xultb_list*list, void*data) {
	struct xultb_list_item*item = xultb_page_get_item_helper(list,data);
	if(item) {
		item->target = data;
		item->vtable = &vtable_xultb_page_item;
	}
	return item;
}

static xultb_bool_t xultb_markup_handle_page_item(struct xultb_list*list, void*target, int flags, int key_code, int x, int y) {
	return XULTB_FALSE;
}

#if 0
/** Searching */
static void do_search() {
	int length = 0;
	if (!searching) {
		buff.setLength(0);
		length = 0;
	} else {
		length = buff.length();
	}
	searching = true;
	if (keyRepeated) {

		// toggle the last character
		buff.setCharAt(length - 1, keys[lastRow][lastCol]);
	} else {

		// do not allow search prefix greater than 4 character
		if (length < 4) {
			buff.append(keys[lastRow][lastCol]);
		}
	}
	xultb_str_t*prefix = buff.toString();
	Window.pushBalloon(prefix, NULL, hashCode(), 1000);
	boolean found = false; // found flag
	final int count = node.getChildCount();
	// traverse the child
	for (int i = 0; i < count && !found; i++) {
		Element elem = (Element) node.getChild(i);
		final int size = OPP_FACTORY_USE_COUNT(&elem->children);
		for (int j = 0; j < size; j++) {
			if (elem.getType(j) == Node.TEXT) {
				if (elem.getText(j).startsWith(prefix)) {
					found = true;
					setSelectedIndex(i);
				}
				break;
			}
		}
	}
}
#endif

static void xultb_page_set_event_listener(struct xultb_page*mlist
		, struct xultb_event_listener*ls) {
	mlist->el = ls;
}

static void xultb_page_set_media_loader(struct xultb_page*mlist
		, struct xultb_media_loader*ml) {
	mlist->ml = ml;
}

OPP_CB(xultb_page);

struct opp_vtable_xultb_page vtable_xultb_page = {
	.set_event_listener = xultb_page_set_event_listener,
	.set_media_loader = xultb_page_set_media_loader,
	.set_node = xultb_page_set_node,
	.oppcb = OPP_CB_FUNC(xultb_page),
};

static struct opp_vtable_xultb_list vtable_xultb_list_overloaded;
OPP_CB(xultb_page) {
	struct xultb_page*mlist = (struct xultb_page*)data;
	switch(callback) {
	case OPPN_ACTION_INITIALIZE:
		memset(mlist, 0, sizeof(struct xultb_page));
//		mlist->super_data.vtable = &vtable_xultb_window_list;
		GUI_LOG("Invoking super ..\n");
		{
			va_list apa;
			va_copy(apa, ap);
			GUI_LOG("Invoking super2 ..\n");
			if(vtable_xultb_list.oppcb(&mlist->super_data, OPPN_ACTION_INITIALIZE, cb_data, apa, 0)) {
				va_end(apa);
				SYNC_LOG(SYNC_ERROR, "Failed ..\n");
				return -1;
			}
			va_end(apa);
		}
		GUI_LOG("Set vtable ..\n");
		opp_vtable_set(mlist, xultb_page);
		opp_vtable_set(&mlist->super_data, xultb_list_overloaded);
		opp_indexed_list_create2(&mlist->left_menu, 4);
		GUI_LOG("Created xultb_page\n");
		return 0;
	case OPPN_ACTION_FINALIZE:
		{
			va_list apa;
			if(vtable_xultb_list.oppcb(&mlist->super_data, OPPN_ACTION_FINALIZE, NULL, apa, 0)) {
				va_end(apa);
				return -1;
			}
			va_end(apa);
		}
		break;
	}
	return 0;
}

static struct opp_factory xultb_page_factory;

struct xultb_page*xultb_page_create(xultb_str_t*title
		, xultb_str_t*default_command) {
	return (struct xultb_page*)opp_alloc4(&xultb_page_factory
			, 0, 0, title, default_command);
}

opp_vtable_extern(xultb_list_item);
int xultb_page_system_init() {
	vtable_xultb_list_overloaded = vtable_xultb_list;
	vtable_xultb_list_overloaded.get_items = xultb_page_get_items;
	vtable_xultb_list_overloaded.get_list_item = xultb_page_get_item;
	vtable_xultb_list_overloaded.handle_item = xultb_markup_handle_page_item;
	vtable_xultb_page_item = vtable_xultb_list_item;
	vtable_xultb_page_item.update = update_impl;
	SYNC_ASSERT(!OPP_FACTORY_CREATE(
			&xultb_page_factory
			, 1,sizeof(struct xultb_page)
			, OPP_CB_FUNC(xultb_page)));
	return 0;
}

