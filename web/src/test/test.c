
#include "config.h"
#include "ui/xultb_guicore.h"
#include "ui/page/xultb_pagecore.h"
#include "opp/opp_indexed_list.h"
#include "ui/page/xultb_page.h"
#include "ui/page/xultb_web_controler.h"


C_CAPSULE_START

int main(int argc, char *argv[]) {
	xultb_pagecore_system_init(&argc, argv);

//	xultb_str_t*title = xultb_str_alloc("Test", 4, NULL, 0);
//	xultb_str_t*dc = xultb_str_alloc("quit", 4, NULL, 0);
//	xultb_str_t*run = xultb_str_alloc("run", 3, NULL, 0);
//	xultb_str_t*play = xultb_str_alloc("play", 4, NULL, 0);
//	struct opp_factory menu_commands;
//	opp_indexed_list_create2(&menu_commands, 4);
//	opp_indexed_list_set(&menu_commands, 0, run);
//	opp_indexed_list_set(&menu_commands, 1, play);

	xultb_str_t*url = xultb_str_alloc("test.xml", 8, NULL, 0);
	struct xultb_page*mlist = xultb_page_create(NULL, NULL);
	struct xultb_web_controler*web = xultb_web_controler_create(mlist);
	web->vtable->push_wrapper(web, url, XULTB_FALSE);
//	struct xultb_ml_node*root = xultb_ml_parse("test.xml");
//	SYNC_ASSERT(root);
//	mlist->vtable->set_node(mlist, root, 0);
//	opp_extvt((&mlist->super_data))->show_full(&mlist->super_data.super_data, &menu_commands, dc);

	while(1) {
		usleep(100);
		xultb_guicore_walk(100);
	}
	return 0;
}

C_CAPSULE_END

