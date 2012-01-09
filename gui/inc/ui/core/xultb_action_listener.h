#ifndef XULTB_ACTION_LISTENER_H
#define XULTB_ACTION_LISTENER_H

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
#include "core/config.h"
#include "core/xultb_exttypes.h"

C_CAPSULE_START

struct xultb_action_listener {
	void*cb_data;
	void (*perform_action)(void*cb_data, xultb_str_t*action);
};

C_CAPSULE_END

#endif // XULTB_ACTION_LISTENER_H
