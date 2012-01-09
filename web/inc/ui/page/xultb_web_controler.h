#ifndef XULTB_DEFAULT_WEB_CONTROLER_H
#define XULTB_DEFAULT_WEB_CONTROLER_H
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

//public class DefaultComplexListener implements EventListener, MediaLoader, ActionListener, ResourceListener, TextEditListener {

#include "ui/page/xultb_media_loader.h"
#include "ui/page/xultb_event_listener.h"
#include "pull_parser/xultb_parser_core.h"
#include "ui/page/xultb_page.h"
#include "opp/opp_type.h"
#include "io/xultb_resource.h"

C_CAPSULE_START

opp_vtable_declare(xultb_web_controler,
	xultb_bool_t (*push_wrapper)(struct xultb_web_controler*web, xultb_str_t*url, xultb_bool_t back);
);

opp_class_declare(xultb_web_controler,
	struct xultb_page*mlist;
	struct xultb_media_loader ml;
	struct xultb_event_listener el;
	struct xultb_action_listener al;
	struct opp_factory stack;
	struct opp_factory images;
	xultb_bool_t loading_page;
	xultb_bool_t is_going_back;
	xultb_str_t*current_url;
	xultb_str_t*base;
	struct xultb_ml_node*root;
	struct opp_vtable_xultb_resource_id resource_vtable;
);

int xultb_list_item_attr_is_positive(struct xultb_ml_node*elem, const char*);
struct xultb_web_controler*xultb_web_controler_create(struct xultb_page*mlist);

int xultb_web_controler_system_init();

C_CAPSULE_END

#endif // XULTB_DEFAULT_WEB_CONTROLER_H
