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

#include "io/xultb_resource.h"
#include "opp/opp_indexed_list.h"
#include "opp/opp_list.h"
#include "ui/core/xultb_action_listener.h"
#include "core/logger.h"
#include "ui/core/xultb_list.h"
#include "ui/page/xultb_web_controler.h"
#include "opp/opp_salt.h"

C_CAPSULE_START

static xultb_str_t*BACK_ACTION = NULL;
static xultb_str_t*MINI_WEB = NULL;

int xultb_list_item_attr_is_positive(struct xultb_ml_node*elem, const char*response) {
	xultb_str_t*value = xultb_ml_get_attribute_value(elem, response);
	return (value && xultb_str_equals_static(value, "y"));//(value && value->len == 1 && *(value->str) == 'y');
}

static int get_default_selected_item(struct xultb_ml_node*node) {
	int i;
	struct xultb_ml_node*s;
	int ret = 0;
	for(i=0;i>0;i++) {
		opp_at_ncode2(s, struct xultb_ml_node*, (&node->children), i,
			if (s->elem.type == XULTB_ELEMENT_NODE && xultb_list_item_attr_is_positive(s, "s")) {
				ret = i;
				i = -2;
			}
		) else {
			break;
		}
	}
	return ret;
}

xultb_bool_t push_wrapper_full(struct xultb_web_controler*web, struct xultb_resource_id*id, xultb_bool_t back) {
	if (web->loading_page) { // check if we are on action ..
		SYNC_LOG(SYNC_VERB,
				"We are still loading a page, we cannot perform other actions ..\n");
		return XULTB_FALSE;
	}
#if 0
	Window.pushBalloon("Loading ..", null, hashCode(), 100000000);
#endif

	// web->images.clear(); -> clear ..
	web->loading_page = XULTB_TRUE;
	web->is_going_back = back;

	xultb_resource_engine_push(id);
	return XULTB_TRUE;
}

xultb_bool_t push_wrapper_impl(struct xultb_web_controler*web, xultb_str_t*url, xultb_bool_t back) {
	return push_wrapper_full(web, xultb_resource_id_create(web->base, url
			, &web->resource_vtable), back);
}

static void submit_form(struct xultb_ml_node*root, xultb_str_t*action) {
#if 0
	// It is dynamic action
	if(doc == null) {
		return;
	}
	try {
		Element x = doc.getRootElement();
		int count = x.getChildCount();
		for(int i=0; i<count; i++) {

			if(x.getType(i) != Node.ELEMENT) {
				continue;
			}
			// find a command
			Element cmd = x.getElement(i);
			String name = cmd.getName();
			if(!name.equals("c")) {
				continue;
			}

			// find the command match our action
			String label = cmd.getText(0);
			if(label == null) {
				continue;
			}
			if(!label.equals(action)) {
				continue;
			}

			// find the reference of the command
			String ref = cmd.getAttributeValue("href");
			if(ref == null) {
				return;
			}
			SimpleLogger.debug(this, "submitForm() => matched: " + ref);

			ResourceIdentifier id = new ResourceIdentifier(base, ref);

			// put the data fields
			Element list = x.getElement(null, "ls");
			int elemCount = list.getChildCount();

			// XXX We are assuming that the radio buttons with same name will be adjacent .. so we are using a radioCheckedFlag .. It will not work on random radio button position of different radio button group ..
			boolean radioCheckedFlag = true;
			for(int j=0; j < elemCount; j++) {
				Element elem = (Element)list.getChild(j);
				final String tag = elem.getName();
				final String key = elem.getAttributeValue("n");
				SimpleLogger.debug(this, "submitForm() => working on: " + elem.getName());
				if(key == null) {
					continue;
				}

				if(tag.equals("ch")) {

					SimpleLogger.debug(this, "submitForm() => key:" + key);

					if(DefaultComplexListener.isPositiveAttribute(elem, "c")) {
						SimpleLogger.debug(this, "submitForm() => value:checked");
						id.put(key, String.valueOf(true));
					} else {
						SimpleLogger.debug(this, "submitForm() => value:not checked");
					}
					continue;
				}

				if(tag.equals("r")) {

					// see if the key is already set ..
					if(id.get(key) != null) {
						continue;
					}

					String value = elem.getAttributeValue("v");
					if(value != null && (radioCheckedFlag = DefaultComplexListener.isPositiveAttribute(elem, "c"))) {
						SimpleLogger.debug(this, "submitForm() => key:" + key);
						SimpleLogger.debug(this, "submitForm() => value:" + value);
						id.put(key, value);
					}
					continue;
				}

				if(tag.equals("s")) {

					SimpleLogger.debug(this, "submitForm() => key:" + key);
					// search selected item ..
					final int opCount = elem.getChildCount();
					boolean isMultipleSelection = isPositiveAttribute(elem, "m");
					String value = "";
					for(int k=0, p=0; k<opCount; k++) {
						Element op = elem.getElement(k);

						// see if it is selected
						if(DefaultComplexListener.isPositiveAttribute(op, "s")) {

							// found selected item
							value = op.getText(0);
							SimpleLogger.debug(this, "submitForm() => value:" + value);
							if(isMultipleSelection) {

								// \xxx String concatanation optimization needed here ..
								id.put(key + '[' + p + ']', value);
							} else {
								id.put(key, value);
								break;
							}
							p++;
						}
					}

					// see if this field is enforced
					if(value.length() == 0 && DefaultComplexListener.isPositiveAttribute(elem, "r")) {
						// so this field is mandatory ..
						Window.pushBalloon(key + " cannot be empty", null);
						return;
					}

					continue;
				}

				if(tag.equals("t")) {

					SimpleLogger.debug(this, "submitForm() => key:" + key);

					// get the text
					String value = null;
					if(elem.getChildCount() == 0 || (value = elem.getText(0)) == null) {

						// see if this field is enforced
						if(DefaultComplexListener.isPositiveAttribute(elem, "r")) {
							// so this field is mandatory ..
							Window.pushBalloon(key + " cannot be empty", null);
							return;
						}

						// do not allow null
						value = "";
					} else {

						// get rid of spaces
						value = value.trim();
					}

					SimpleLogger.debug(this, "submitForm() => value:" + value);

					id.put(key, value);
					continue;
				}
			}

			// see if there is a checked value for the radio button groups ..
			if(!radioCheckedFlag) {
				Window.pushBalloon("Please check the radio buttons ", null);
				return;
			}

			// see if the form data should be preserved ..
			if(isPositiveAttribute(x, "cf")) {

				try {
					// delete the old
					Document.delete(MINI_WEB, null, doc.getRecordId(), false);

					// save the updated one
					doc.store(MINI_WEB, doc.getUrl(), false, 1000000); // FIXME the timeout is not set correctly
				} catch(Exception e) {
					SimpleLogger.error(this, "submitForm()\t\tCould not backup form.", e);
				}
			}
			push_wrapper_impl(id, false);
			return;
		}
	} catch(RuntimeException e) {
		SimpleLogger.debug(this, "submitForm() => no commands", e);
	}
#endif
}


void perform_action_impl(void*cb_data, xultb_str_t*action) {
	struct xultb_web_controler*web = (struct xultb_web_controler*)cb_data;
	SYNC_LOG(SYNC_VERB, "Performing action\n");
#if 0
	// selection box mode ....
	if(showingSelectionBox != null) {

		// see if it supports multiple selection ..
		if(isPositiveAttribute(showingSelectionBox, "m")) {

			// so it is multiple selection box ..

			// see if he is toggling ..
			if(action != OK) {

				// toggle selection
				setOptionInSelectionBox(showingSelectionBox, ml.getSelectedIndex(), true);
				return;
			}

			// otherwise he is going back ..
		} else {

			// set the selection index ..
			setOptionInSelectionBox(showingSelectionBox, ml.getSelectedIndex(), false);
		}

		// get back to the old page ..
		Element ls = currentDoc.getElement(null, "x").getElement(null, "ls");
		ml.setNode(ls, get_default_selected_item(ls));

		// fix the title
		String title = ls.getAttributeValue("t");
		if(title == null) {
			title = MINI_WEB;
		}
		ml.setTitle(title);

		// fix the selected index
		ml.setSelectedIndex(oldSelection);

		// fix the menu
		ml.show();
		showingSelectionBox = null;
		return;
	}
#endif

	// Normal mode ..
	if(action == web->mlist->super_data.default_command) {
		SYNC_LOG(SYNC_VERB, "item action !\n");
		struct xultb_ml_node*elem = (struct xultb_ml_node*)web->mlist->super_data.vtable->get_selected(&web->mlist->super_data);
		xultb_str_t*ref = xultb_ml_get_attribute_value(elem, "href");
		if(ref) {
			push_wrapper_impl(web, ref, XULTB_FALSE);
			return;
		}

#if 0
		// so it is not link see if it is text field ..
		final String tag = elem.getName();
		if(tag.equals("t")) {

			// get the text
			String text = null;
			if(elem.getChildCount() == 0 || (text = elem.getText(0)) == null) {

				// do not allow null
				text = "";
			}

			// create renderer
			TextEdit.getInstance().showTextEdit(this, elem.getAttributeValue("l"), text, 100, (isPositiveAttribute(elem, "p"))? TextField.PASSWORD : TextField.ANY);
		} else if(tag.equals("s")) {

			// we render the selection box
			showingSelectionBox = elem;

			// \todo override the title
			ml.setTitle(elem.getAttributeValue("l"));
			ml.setNode(elem, 0);

			// save the old selected index
			oldSelection = ml.getSelectedIndex();

			// set selected index
			final int count = showingSelectionBox.getChildCount();
			for(int i=0; i<count; i++) {
				Element elem = showingSelectionBox.getElement(i);
				// see if it is selected
				if(DefaultComplexListener.isPositiveAttribute(elem, "s")) {
					ml.setSelectedIndex(i);
				}
			}

			if(isPositiveAttribute(showingSelectionBox, "m")) {

				// override the menu , show OK
				Menu.getInstance().setMenu(OK, null);
			} else {

				// override the menu and show only go !
				Menu.getInstance().setMenu(null, null);
			}

			ml.repaint();
		} else if(tag.equals("ch")){

			// toggle checkbox state ..
			if(DefaultComplexListener.isPositiveAttribute(elem, "c")) {
				elem.setAttribute(null, "c", null);
			} else {
				elem.setAttribute(null, "c", "y");
			}
			ml.repaint();
		} else if(tag.equals("r")) {

			if(DefaultComplexListener.isPositiveAttribute(elem, "c")) {
				return;
			}

			// get the radio button name ..
			String name = elem.getAttributeValue("n");
			if(name == null) {
				return;
			}

			// find other radio buttons with the same name ..
			Element ls = (Element)elem.getParent();
			Element each = null;
			String eachName = null;
			int count = ls.getChildCount();

			// traverse all the items
			for(int i=0; i < count; i++) {

				each = (Element)ls.getChild(i);

				SimpleLogger.debug(this, "performAction()\t\tElement -> " + count + ":" + i + ":" + each.getName());
				// skip the current item ..
				if(each == elem) {
					continue;
				}

				// we are only interested about the radio buttons
				if(!each.getName().equals("r")) {
					continue;
				}

				// mathch the group
				eachName = each.getAttributeValue("n");
				if(eachName == null || !name.equals(eachName)) {
					continue;
				}

				SimpleLogger.debug(this, "performAction()\t\tunchecking " + each.getAttributeValue("l"));
				// uncheck
				each.setAttribute(null, "c", null);
			}

			elem.setAttribute(null, "c", "y");

			ml.repaint();
		}
#endif
	} else if(action == BACK_ACTION) {
		SYNC_LOG(SYNC_VERB, "Back back back .. \n");
		xultb_str_t*last = opp_indexed_list_get(&web->stack, OPP_FACTORY_USE_COUNT(&web->stack) - 1);
		if(last) {
			SYNC_LOG(SYNC_VERB, "It should render %s\n", last->str);
			OPPUNREF(web->base);
			push_wrapper_impl(web, last, XULTB_TRUE);
			OPPUNREF(last);
		}
	} else {
		submit_form(web->mlist->root, action);
	}
}

void handle_event_impl(void*cb_data, struct xultb_ml_node*elem, enum markup_event_type type) {
	SYNC_LOG(SYNC_VERB, "TODO render specified url\n");
	push_wrapper_impl(cb_data, xultb_ml_get_attribute_value(elem, "href"), XULTB_FALSE);
	// pushWrapper(elem.getAttributeValue("href"), false);
}

struct xultb_img*get_image_impl(void*cb_data) {
	return NULL;
}

static void clear_flags(struct xultb_web_controler*web) {
	web->loading_page = XULTB_FALSE;
	web->is_going_back = XULTB_FALSE;
#if 0
	Window.pushBalloon(null, null, hashCode(), 0);
#endif
}

#define WEB_ASSERT_RETURN(x,y,z) if(!x) {SYNC_LOG(SYNC_VERB, y); return z;}
static void handle_content_impl(void*cb_data, xultb_str_t*url, enum xultb_resource_type type, void*obj) {
	struct xultb_web_controler*web = (struct xultb_web_controler*)cb_data;
	// \todo set menu command ..
	if(type == XULTB_RESOURCE_IMG) {
#if 0
		Window.pushBalloon(null, null, hashCode(), 0);
#endif
		SYNC_LOG(SYNC_VERB, "handleContent()\t\t[+]Image -> %s\n", url->str);
		OPP_ALLOC2(&web->images, obj);
		xultb_guicore_set_dirty(&web->mlist->super_data.super_data);
		return;
	}
	struct xultb_ml_node*root = obj;
	struct xultb_ml_node*x = xultb_ml_get_node(root, "x");
	WEB_ASSERT_RETURN(x, "no x\n",);
	struct xultb_ml_node*list = xultb_ml_get_node(x, "ls");
	WEB_ASSERT_RETURN(list, "no ls\n",);
#if 0
	Window.pushBalloon(null, null, hashCode(), 0);
#endif
#if 0
	synchronized (this) {
#endif
	if(web->current_url && !web->is_going_back) {
		if(xultb_strcmp(web->current_url, url)) {
			SYNC_LOG(SYNC_VERB, "handleContent()\t\t%s\t\t[+]\n", web->current_url->str);
			opp_indexed_list_set(&web->stack, OPP_FACTORY_USE_COUNT(&web->stack), web->current_url);
		} else {
			SYNC_LOG(SYNC_VERB, "handleContent()\t\t%s\t\t[*]\n", web->current_url->str);
		}
	} else if(web->is_going_back){
		// SYNC_LOG(SYNC_VERB, "handleContent()\t\t" + stack.lastElement() + "\t\t[-]");
		opp_indexed_list_set(&web->stack,OPP_FACTORY_USE_COUNT(&web->stack)-1, NULL);
	}
	OPPUNREF(web->current_url);
	web->current_url = OPPREF(url);

	// save the current doc and base
	web->root = OPPREF(root);
	OPPUNREF(web->base);
	web->base = xultb_str_alloc(NULL, 128, NULL, 0);

	int last = xultb_str_indexof_char(url, '/');
	if(last == -1) {
//		xultb_str_cat(web->base, url);
//		xultb_str_cat_char(web->base, '/');
	} else {
		xultb_str_cat(web->base, url);
		web->base->len = last+1;
	}
#if 0
	}
	reset_menu(web);
#endif
	int i = 0, j = 0, k = 0;
	struct xultb_ml_node*menu,*cmd;
	web->mlist->right_menu = NULL;
	if (OPP_FACTORY_USE_COUNT(&web->stack)) {
		web->mlist->right_menu = BACK_ACTION;
	}
	for(i=0;;i++) {
		opp_at_ncode(menu, (&x->children), i,
			if(menu->elem.type == XULTB_ELEMENT_NODE && xultb_str_equals_static(menu->name, "m")) {
				for(j = 0;; j++) {
				opp_at_ncode(cmd, (&menu->children), j,
					xultb_str_t*target = xultb_ml_get_text(cmd);
					if(target) {
						if(!web->mlist->right_menu) {
							GUI_LOG("Adding right command %s\n", target->str);
							web->mlist->right_menu = target;
						} else {
							GUI_LOG("Adding command %s\n", target->str);
							opp_indexed_list_set(&web->mlist->left_menu, k++, target);
						}
					}
				) else {
					break;
				}
				}
			}
		) else {
			break;
		}
	}
#if 0
	String tmp = x.getAttributeValue("c");
	long cacheTimeout = (tmp != null)?(Integer.parseInt(tmp)*1000):-1;
	// See if we can cache it
	if(cacheTimeout != -1) {

		// see if it already cached
		if(!Document.exists(MINI_WEB, url)) {

			SimpleLogger.debug(this, "handleContent()\t\t[######] <<  " + url);
			// cache it
			doc.store(MINI_WEB, url, false, cacheTimeout);
		}

	}
#endif
	// SYNC_LOG(SYNC_ERROR, "handleContent(): no commands\n");
	xultb_str_t*title = xultb_ml_get_attribute_value(list, "t");
	if(!title) {
		title = MINI_WEB;
	}
	web->mlist->super_data.super_data.vtable->set_title(&web->mlist->super_data.super_data, title);
	web->mlist->vtable->set_node(web->mlist, list, get_default_selected_item(list));
//	web->mlist->super_data.super_data.vtable->show_full(&web->mlist->super_data.super_data
//			, left_menu, right_menu);
	clear_flags(web);
}

static void handle_content_error_impl(void*cb_data, xultb_str_t*url, enum xultb_resource_type type, int code, xultb_str_t*reason) {
	struct xultb_web_controler*web = (struct xultb_web_controler*)cb_data;
	clear_flags(web);
    SYNC_LOG(SYNC_VERB, "handleError()\n");
    if(type == XULTB_RESOURCE_DOM) {
    	// what to do ??
    } else {
//    images.put(url, Image.createImage("/ui/error.png"));
    }
#if 0
    Window.pushBalloon("Error ..", null, hashCode(), 2000);
#endif
}


struct opp_vtable_xultb_web_controler vtable_xultb_web_controler = {
		.push_wrapper = push_wrapper_impl,
		.oppcb = NULL,
};

static struct opp_factory web_controlers;
struct xultb_web_controler*xultb_web_controler_create(struct xultb_page*mlist) {
	struct xultb_web_controler*web = (struct xultb_web_controler*)OPP_ALLOC2(&web_controlers, mlist);
	web->mlist = mlist;
	web->al.perform_action = perform_action_impl;
	web->al.cb_data = web;
	web->el.handle_event = handle_event_impl;
	web->el.cb_data = web;
	web->ml.get_image = get_image_impl;
	web->ml.cb_data = web;
	opp_extvt(mlist)->set_action_listener((&mlist->super_data), &web->al);
	mlist->vtable->set_media_loader(mlist, &web->ml);
	mlist->vtable->set_event_listener(mlist, &web->el);
	opp_indexed_list_create2(&web->stack, 4);
	opp_list_create2(&web->images, 4, OPPF_HAS_LOCK | OPPF_SWEEP_ON_UNREF | OPPF_SEARCHABLE | OPPF_EXTENDED);
	web->loading_page = XULTB_FALSE;
	web->is_going_back= XULTB_FALSE;
	web->base = web->current_url = NULL;
	web->root = NULL;
	opp_vtable_set(web, xultb_web_controler);
	web->resource_vtable.cb_data = web;
	web->resource_vtable.handle_content = handle_content_impl;
	web->resource_vtable.handle_error = handle_content_error_impl;
	return web;
}

int xultb_web_controler_system_init() {
	MINI_WEB = xultb_str_clone("Web", 3, 0);
	BACK_ACTION = xultb_str_clone("Back", 4, 0);
	SYNC_ASSERT(!OPP_FACTORY_CREATE(
		&web_controlers
		, 1,sizeof(struct xultb_web_controler)
		, NULL)
	);
	return 0;
}

C_CAPSULE_END

