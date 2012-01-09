#ifndef XULTB_MENU_H
#define XULTB_MENU_H

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

#include "core/xultb_decorator.h"
#include "core/xultb_exttypes.h"
#include "opp/opp_factory.h"
#include "ui/core/xultb_graphics.h"

C_CAPSULE_START

enum {
	XULTB_MENU_PADDING = 3,
};

struct xultb_window;
void xultb_menu_paint(struct xultb_graphics*g, int width, int height);
#if 1
int xultb_menu_get_base_height();
struct xultb_font*xultb_menu_get_base_font();
#endif
xultb_bool_t xultb_menu_is_active();
int xultb_menu_set(struct opp_factory*left_option, xultb_str_t*right_option);
int xultb_menu_handle_event(struct xultb_window*win, void*target, int flags, int key_code, int x, int y);
int xultb_menu_system_init();


C_CAPSULE_END

#endif // XULTB_MENU_H
