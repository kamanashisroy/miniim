/*
 * xultb_gui_input.h
 *
 *  Created on: Dec 26, 2011
 *      Author: ayaskanti
 */

#ifndef XULTB_GUI_INPUT_H_
#define XULTB_GUI_INPUT_H_


#include "config.h"

C_CAPSULE_START

enum {
	XULTB_INPUT_KEYBOARD_EVENT = 1,
	XULTB_INPUT_SCREEN_EVENT = (1<<4), // touch / mouse event
};

enum {
	XULTB_INPUT_KEY_UP = 1,
	XULTB_INPUT_KEY_DOWN = 2,
	XULTB_INPUT_KEY_ENTER = 3,
};

//#define GUI_INPUT_LOG(...) SYNC_LOG(SYNC_VERB, __VA_ARGS__)
#define GUI_INPUT_LOG(...)

struct xultb_window;
int xultb_gui_input_register_action(void*data, int x, int y, int width, int height);
int xultb_gui_input_reset(struct xultb_window*win); // This should be called before registering action
int xultb_gui_input_init();
int xultb_gui_input_platform_init(int (*handle_event)(int flags, int key_code, int x, int y));

C_CAPSULE_END

#endif /* XULTB_GUI_INPUT_H_ */
