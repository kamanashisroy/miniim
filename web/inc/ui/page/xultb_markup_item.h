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
#include "ui/page/xultb_media_loader.h"

C_CAPSULE_START

struct xultb_list_item*xultb_markup_item_create(struct xultb_ml_node*root
		, struct xultb_media_loader*loader, xultb_bool_t selectable
		, struct xultb_event_listener*el);

int xultb_markup_item_system_init();
int xultb_markup_item_system_deinit();

C_CAPSULE_END

#endif // XULTB_MARKUP_ITEM_H
