/*
 * xultb_guicore.c
 *
 *  Created on: Jan 14, 2011
 *      Author: ayaskanti
 */

#include "config.h"
#include "core/logger.h"
#include "opp/opp_queue.h"
#include "opp/opp_list.h"
#include "opp/opp_iterator.h"
#include "core/xultb_string.h"
#include "ui/xultb_guicore.h"
#include "ui/xultb_gui_input.h"
#include "opp/opp_any_obj.h"

C_CAPSULE_START

static struct opp_queue painter_queue;
static struct xultb_graphics*gr;
static struct opp_factory tasks;
int xultb_guicore_system_init(int*argc, char *argv[]) {
	opp_any_obj_system_init();
	xultb_guicore_platform_init(argc, argv);
	xultb_str_system_init();
	xultb_graphics_system_init();
	xultb_font_system_init();
	xultb_window_system_init();
	xultb_list_item_system_init();
	xultb_list_system_init();
	opp_queuesystem_init();
	xultb_gui_input_init();
	opp_queue_init2(&painter_queue, 0);
	opp_list_create2(&tasks, 4, 0);
	gr = xultb_graphics_create();
	return 0;
}

int xultb_guicore_set_dirty(struct xultb_window*win) {
	opp_enqueue(&painter_queue, win);
	return 0;
}

int xultb_guicore_register_task(struct xultb_gui_task*task) {
	opp_alloc4(&tasks, 0, 0, task);
	return 0;
}

int xultb_guicore_unregister_task(struct xultb_gui_task*task) {
	return opp_list_prune(&tasks, task, OPPN_ALL, 0, 0);
}

static int xultb_perform_tasks(void*data, void*func_data) {
	struct xultb_gui_task*task = ((struct opp_list_item*)data)->obj_data;
	int ms = *((int*)func_data);
	SYNC_ASSERT(task);
	task->cb_run(task, ms);
	return 0;
}

int xultb_guicore_walk(int ms) {
	opp_factory_do_full(&tasks, xultb_perform_tasks, &ms, OPPN_ALL, 0, 0);
	xultb_guicore_platform_walk(ms);
	// see if we need to refresh anything ..
	while(1) {
		struct xultb_window*win = (struct xultb_window*)opp_dequeue(&painter_queue);
		if(!win) {
			break;
		}
		// TODO repaint a window only once ..
		GUI_LOG("Painting window\n");
		xultb_gui_input_reset(win);
		gr->start(gr);
		win->vtable->paint(win, gr);
		OPPUNREF(win);
	}
	xultb_guicore_platform_walk(ms);
	return 0;
}

C_CAPSULE_END
