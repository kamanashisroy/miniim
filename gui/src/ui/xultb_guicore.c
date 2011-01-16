/*
 * xultb_guicore.c
 *
 *  Created on: Jan 14, 2011
 *      Author: ayaskanti
 */

#include "config.h"
#include "ui/xultb_guicore.h"

int xultb_guicore_system_init() {
	xultb_list_item_system_init();
	xultb_list_system_init();
	xultb_window_system_init();
	return 0;
}
