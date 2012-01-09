/*
 * xultb_guicore.c
 *
 *  Created on: Jan 14, 2011
 *      Author: ayaskanti
 */

#include "config.h"
#include "core/logger.h"
#include "opp/opp_queue.h"
#include "ui/page/xultb_pagecore.h"
#include "ui/page/xultb_page.h"
#include "ui/page/xultb_web_controler.h"
#include "ui/xultb_guicore.h"
#include "pull_parser/xultb_parser_core.h"
#include "io/xultb_resource.h"
#include "ui/page/xultb_markup_item.h"

C_CAPSULE_START

int xultb_pagecore_system_init(int*argc, char *argv[]) {
	xultb_guicore_system_init(argc, argv);
	xultb_page_system_init();
	xultb_markup_item_system_init();
	xultb_parser_system_init();
	xultb_resource_engine_init();
	xultb_web_controler_system_init();
//	xultb_markup_item_system_init();
	return 0;
}

int xultb_pagecore_system_deinit() {
	// guicore deinit
	return 0;
}

C_CAPSULE_END
