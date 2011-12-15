/*
 * xultb_guicore.c
 *
 *  Created on: Jan 14, 2011
 *      Author: ayaskanti
 */

#include "config.h"
#include "core/logger.h"
#include "opp/opp_queue.h"
#include "ui/xultb_guicore.h"

C_CAPSULE_START

static struct opp_queue painter_queue;
static struct xultb_graphics*gr;
int xultb_guicore_system_init(int*argc, char *argv[]) {
	xultb_guicore_platform_init(argc, argv);
	xultb_graphics_system_init();
	xultb_list_item_system_init();
	xultb_list_system_init();
	xultb_window_system_init();
	xultb_font_system_init();
	opp_queuesystem_init();
	opp_queue_init2(&painter_queue, 0);
	gr = xultb_graphics_create();
	return 0;
}

int xultb_guicore_set_dirty(struct xultb_window*win) {
	opp_enqueue(&painter_queue, win);
	return 0;
}

int xultb_guicore_walk(int ms) {
	// see if we need to refresh anything ..
	while(1) {
		struct xultb_window*win = (struct xultb_window*)opp_dequeue(&painter_queue);
		if(!win) {
			break;
		}
		SYNC_LOG(SYNC_VERB, "Painting window\n");
		win->vtable->paint(win, gr);
		OPPUNREF(win);
	}
	xultb_guicore_platform_walk(ms);
	return 0;
}

C_CAPSULE_END
