#ifndef XULTB_EVENT_LISTENER_H
#define XULTB_EVENT_LISTENER_H

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

C_CAPSULE_START

enum markup_event_type {
	MARKUP_CLICK = 100,
};

struct xultb_event_listener {
	void (*handle_event)(struct xultb_ml_node elem, enum markup_event_type type);
};

C_CAPSULE_END

#endif // XULTB_EVENT_LISTENER_H
