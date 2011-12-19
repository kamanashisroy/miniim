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

#include "ui/core/list/xultb_list_item.h"
#include "ui/page/extended/xultb_markup_list.h"

opp_vtable_extern(xultb_list);

static struct xultb_list_item*getListItem(struct xultb_ml_node*elem) {
	xultb_str_t*name = elem->get_name(elem);
	xultb_str_t*label = elem->get_attribute_value(elem, "l");
	if (!name || xultb_equals_static(name, "m")) {
		return xultb_markup_item_create(elem, ml, XULTB_FALSE, el);
	} else if (name->equals("l")) {
		xultb_str_t*text = ".";
		if (elem->get_child_count() > 0) {
			text = elem->get_text(0);
			if (text != NULL) {
				text = text->trim();
			} else {
				text = ".";
			}
		}

		// see if the label has any image
		struct xultb_img*img = null;
		xultb_str_t src = elem->get_attribute_value("src");
		if (src != NULL) {
			img = ml.get_image(src);
		}
		return xultb_list_item_create_label_full(text, img, elem.getAttributeValue(
				"href") != null, false);
	} else if (name.equals("t")) {

		// get current text
		String text = null;
		if (elem.getChildCount() == 0 || (text = elem.getText(0)) == null) {

			// get hint of this field
			text = elem.getAttributeValue("h");
			if (text == null) {
				text = "";
			}
		} else {
			// get rid of spaces
			text = text.trim();
		}

		// see if it is password field
		if (DefaultComplexListener.isPositiveAttribute(elem, "p")) {

			// in this case we hide the content of the password ..
			text = "**********************************".substring(0,
					text.length());
		}

		// do not scroll continuously when there is textfield
		continuousScrolling = false;
		return xultb_list_item_create_text_input_full(label, text,
				DefaultComplexListener.isPositiveAttribute(elem, "w"), true);
	} else if (name.equals("s")) {

		// get selected index
		StringBuffer buffer = new StringBuffer();
		boolean first = true;
		final int count = elem.getChildCount();
		for (int i = 0; i < count; i++) {
			Element op = elem.getElement(i);

			// see if it is selected
			if (DefaultComplexListener.isPositiveAttribute(op, "s")) {
				String tmp = op.getText(0);
				if (tmp == null) {
					continue;
				}
				if (first) {
					first = false;
				} else {
					buffer.append(',');
				}
				buffer.append(tmp.trim());
			}
		}

		// do not scroll continuously when there is selection box
		continuousScrolling = false;
		return xultb_list_item_create_selection_box(label, buffer.toString(),
				true);
	} else if (name.equals("r")) {
		// render radio button

		return xultb_list_item_create_radio_button(label,
				DefaultComplexListener.isPositiveAttribute(elem, "c"), true);
	} else if (name.equals("ch")) {
		// so it is checkbox

		return xultb_list_item_create_checkbox(label,
				DefaultComplexListener.isPositiveAttribute(elem, "c"), true);
	} else if (name.equals("o")) {
		// so it is selection option
		// get current text
		String text = null;
		if (elem.getChildCount() == 0 || (text = elem.getText(0)) == null) {

			// get hint of this field
			text = elem.getAttributeValue("h");
			if (text == null) {
				text = "";
			}
		} else {

			// get rid of spaces
			text = text.trim();
		}

		// see if it is multiple selection box ..
		if (isMultipleSelection) {

			// see if it is selected ..
			return ListItemFactory.createCheckBox(text,
					DefaultComplexListener.isPositiveAttribute(elem, "s"), true);
		} else {
			return xultb_list_item_create_label_full(text, null, true, false);
		}
	} else {
		return xultb_markup_item_create(elem, ml, false, el);
	}
}

static xultb_bool_t handleElement(int keyCode, int gameAction) {
	int index = super.getSelectedIndex();

	// avoid invalid selected index
	if (index >= node.getChildCount()) {
		return false;
	}

	// allow markup traversing
	Element elem = (Element) node.getChild(index);
	ListItem item = getListItem(elem);
	if (item instanceof MarkupItem) {
		if (((MarkupItem) item).keyPressed(keyCode, gameAction)) {
			item.free();
			return true;
		}
	}
	item.free();

	// see if it is any text
	if (keyCode < Canvas.KEY_NUM0 || keyCode > Canvas.KEY_NUM9) {
		item.free();
		searching = false;
		return false;
	}
	long curTime = System.currentTimeMillis();
	if (curTime - lastEvent > 1000 || (keyCode - Canvas.KEY_NUM0) != lastRow) { // 1 second

		// new key
		lastRow = keyCode - Canvas.KEY_NUM0;
		lastCol = 0;
		keyRepeated = false;
	} else {

		// updated key
		if (++lastCol > keys[lastRow].length) {
			lastCol = 0;
		}
		keyRepeated = true;
	}
	lastEvent = curTime;

	if (item instanceof ListItemFactory) {
		ListItemFactory li = (ListItemFactory) item;

		if (li.getType() == ListItemFactory.TEXT_INPUT) {

			// allow inline text editing
			doEdit(elem);
			item.free();
			return true;
		}
	}

	// allow searching
	doSearch();
	item.free();
	return true;
}

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
	String prefix = buff.toString();
	Window.pushBalloon(prefix, null, hashCode(), 1000);
	boolean found = false; // found flag
	final int count = node.getChildCount();
	// traverse the child
	for (int i = 0; i < count && !found; i++) {
		Element elem = (Element) node.getChild(i);
		final int size = elem.getChildCount();
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

static void do_edit(Element elem) {
    xultb_str_t*oldText = null;
	int count = elem.getChildCount();
	for (int i = 0; i < count; i++) {
		oldText = elem.getText(0);
		elem.removeChild(0);
	}
	buff.setLength(0);
	if (oldText != null) {
		buff.append(oldText);
	}

	if (keyRepeated) {
		buff.setCharAt(buff.length() - 1, keys[lastRow][lastCol]);
	} else {
		buff.append(keys[lastRow][lastCol]);
	}

	// so we have solid text
	elem.addChild(Node.TEXT, buff.toString());
	buff.setLength(0);
}


struct opp_vtable_xultb_markup_list vtable_xultb_markup_list = {
	.set_event_listener = xultb_markup_list_set_event_listener,
	.set_media_loader = xultb_markup_list_set_media_loader,
};


OPP_CB(xultb_markup_list) {
	struct xultb_markup_list*mlist = (struct xultb_markup_list*)data;
	// do a cleanup
	memset(mlist, 0, sizeof(struct xultb_markup_list));
	switch(callback) {
	case OPPN_ACTION_INITIALIZE:
//		mlist->super_data.vtable = &vtable_xultb_window_list;
		{
			va_list apa;
			va_copy(apa, ap);
			if(vtable_xultb_window.oppcb(&mlist->super_data, OPPN_ACTION_INITIALIZE, cb_data, apa)) {
				va_end(apa);
				return -1;
			}
			va_end(apa);
		}
		opp_vtable_set(mlist, xultb_markup_list);
		SYNC_LOG(SYNC_VERB, "Created xultb_markup_list\n");
		return 0;
	case OPPN_ACTION_FINALIZE:
		opp_factory_destroy(&list->_items);
		{
			va_list ap;
			if(vtable_xultb_window.oppcb(&mlist->super_data, OPPN_ACTION_FINALIZE, NULL, ap)) {
				return -1;
			}
		}
		break;
	}
	return 0;
}

static struct opp_factory xultb_markup_list_factory;

struct xultb_markup_list*xultb_markup_list_create(xultb_str_t*title, xultb_str_t*default_command) {
	return (struct xultb_markup_list*)opp_alloc4(&xultb_markup_list_factory, 0, 0, title, default_command);
}

static int xultb_markup_list_system_init() {
//	vtable_xultb_window_list = vtable_xultb_window;
//	vtable_xultb_window_list.paint = xultb_list_window_paint_wrapper;
	SYNC_ASSERT(OPP_FACTORY_CREATE(
			&xultb_markup_list_factory
			, 1,sizeof(struct xultb_markup_list)
			, OPP_CB_FUNC(xultb_markup_list)) == 0);
	return 0;
}

#if 0
static int xultb_markup_list_finalize() {
	return 0;
}
#endif
