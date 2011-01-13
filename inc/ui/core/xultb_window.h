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

struct graphics;

struct xultb_window {
	int PADDING;
	xultb_str_t title;
	xultb_font_t*TITLE_FONT;
	
	/** The width of the list */
	int width;
	int halfWidth;

	/** The height of the list */
	/** Menu start position by pixel along Y-axis */
	int height;
	int menuY;
	int panelTop;
	
	void (*init)(struct xultb_window*win, int w, int h);
	void (*show)(struct xultb_window*win);
	void (*show_full)(struct xultb_window*win, xmltb_str_t*right_option, xultb_str_t**left_option, int left_option_count);
	xmltb_bool_t* (*is_showing)(struct xultb_window*win);
#if 0
	void (*repaint)();
	void (*repaint_full)(int x, int y, int width, int height);
	xmltb_bool_t* (*handle_menu)(int key_code, int game_action);
	void (*setTitle)(xultb_bool_t* title);
	xmltb_str_t* (*get_title)();
#endif

	void (*paint)(struct xultb_window*win, struct xultb_graphics*g);

#if 0
	// what is this for ??
	void (*push_balloon)(xultb_str_t message, xultb_img_t img, int hash, long timeout);
	void (*push_balloon)(xultb_str_t message, xultb_img_t img);
#endif
	xul_window_t*(*get_current)();
};

struct xultb_window*create_xultb_window(xultb_str_t*title);