/*
 * guicore.h
 *
 *  Created on: Jan 14, 2011
 *      Author: ayaskanti
 */

#ifndef XULTB_GUICORE_H
#define XULTB_GUICORE_H

#include "core/xultb_decorator.h"
#include "core/xultb_exttypes.h"
#include "core/xultb_obj_factory.h"

#include "opp/opp_indexed_list.h"

#include "ui/core/xultb_img.h"
#include "ui/core/xultb_graphics.h"
#include "ui/core/xultb_window.h"
#include "ui/core/xultb_action_listener.h"
#include "ui/core/xultb_menu.h"
#include "ui/core/xultb_list.h"
#include "ui/core/list/xultb_list_item.h"

C_CAPSULE_START

enum {
	OPPN_ACTION_GUI_RENDER = 1024,
};

struct xultb_gui_task {
	int (*cb_run)(void*cb_data, int ms);
};

//#define GUI_LOG(...) SYNC_LOG(SYNC_VERB, __VA_ARGS__)
#define GUI_LOG(...)
/*
 * Some library initialization code especially
 * qt requires the exact argc address passsed into
 * the main() function.
 * */
int xultb_guicore_system_init(int*argc, char *argv[]);
int xultb_guicore_platform_init(int*argc, char *argv[]);
int xultb_guicore_walk(int ms);
int xultb_guicore_register_task(struct xultb_gui_task*task);
int xultb_guicore_unregister_task(struct xultb_gui_task*task);
int xultb_guicore_platform_walk(int ms);
int xultb_guicore_set_dirty(struct xultb_window*win);
#define xultb_guicore_set_dirty2(win, x, y, width, height) xultb_guicore_set_dirty(win)
C_CAPSULE_END

#endif /* XULTB_GUICORE_H */
