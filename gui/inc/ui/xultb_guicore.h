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

/*
 * Some library initialization code especially
 * qt requires the exact argc address passsed into
 * the main() function.
 * */
int xultb_guicore_system_init(int*argc, char *argv[]);
int xultb_guicore_platform_init(int*argc, char *argv[]);
int xultb_guicore_platform_show(struct xultb_window*win);
int xultb_guicore_platform_walk(int ms);

C_CAPSULE_END

#endif /* XULTB_GUICORE_H */
