/*
 * xultb_guicore.c
 *
 *  Created on: Jan 14, 2011
 *      Author: ayaskanti
 */

#include "config.h"
#include "core/logger.h"
#include "ui/xultb_guicore.h"

C_CAPSULE_START

int xultb_guicore_system_init(int*argc, char *argv[]) {
	xultb_str_t title = xultb_str_create("Xultube");

	xultb_guicore_platform_init(argc, argv);
	xultb_graphics_system_init();
	xultb_list_item_system_init();
	xultb_list_system_init();
	xultb_window_system_init();

	struct xultb_window*win = xultb_window_create(&title);

	win->width = 10;
	SYNC_LOG(SYNC_VERB, "Window title:%s,width:%d,height:%d\n"
			, win->title.str
			, win->width, win->height);

	xultb_guicore_platform_show(win);

	xultb_guicore_platform_run();
	return 0;
}

C_CAPSULE_END
