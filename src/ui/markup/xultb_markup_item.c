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

struct xultb_markup_item {

	struct list_item list_item;
	struct element root; // Document root
	/**
	 * y-coordinate position of the image
	 */
	int xPos; // = 0
	int yPos;// = 0;

	struct graphics g;// = null;

	int lineHeight; // = 0;
	int width;
	int minLineHeight;// = Font.getFont(Font.FACE_SYSTEM, Font.STYLE_PLAIN,Font.SIZE_SMALL).getHeight()+PADDING;
	boolean selected;// = false;
	media_loader loader;// = null;
	markup_item single_instance;// = null;

	int (*initialize)(void*data);
	void (*render_node)(struct font*font, struct element*elem);
	void (*render_text)(struct font*font, const char*text);
	void (*render_image)(struct element*elem);
	void (*break_line)();
	void (*clear_line)();
	void (*clear_line)(int y, int height);
	void (*update_height)(struct font*font);
	void (*update_height)(int newHeight);

	/** List item implementation .. */
	/*@{*/
	int (*paint)(struct graphics g, int x, int y, int width, boolean selected);
	/*@}*/

	void (*free)();
	
	/// traverser
	/*@{*/
	void set_listener(struct event_listener listener);
	
	void (*free_traverser)();
	/** Key press listener to traverse the browsable elements */
	int (*key_pressed)(int key_code, int game_action);
	void (*search_focusable_elements)(struct element*elem);
	void (*set_focus)(struct element*elem);
	int (*is_focused)(struct element*elem);
	int (*is_active)(struct element*elem);
	/*@}*/
}

struct markup_item*get_instance(struct element root, struct media_loader loader, boolean selectable, struct event_listener el);
