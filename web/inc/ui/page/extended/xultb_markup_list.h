#ifndef XULTB_MARKUP_LIST_H
#define XULTB_MARKUP_LIST_H
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
#include "ui/xultb_guicore.h"
#include "ui/page/xultb_event_listener.h"
#include "ui/page/xultb_media_loader.h"

C_CAPSULE_START

opp_vtable_declare(xultb_markup_list,
	void (*set_event_listener)(struct xultb_event_listener*ls);
	void (*set_media_loader)(struct xultb_media_loader*ml);
);

opp_class_declare(xultb_markup_list,
	opp_class_extend(struct xultb_list);
);

struct xultb_markup_list*xultb_markup_list_create(xultb_str_t*title, xultb_str_t*default_command);
int xultb_markup_list_system_init();

C_CAPSULE_END

#endif // XULTB_MARKUP_LIST_H
