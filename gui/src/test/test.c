
#include "config.h"
#include "ui/xultb_guicore.h"
#include "ui/core/xultb_window.h"

C_CAPSULE_START

int main(int argc, char *argv[]) {

	xultb_str_t title = xultb_str_create("Test");
	xultb_str_t dc = xultb_str_create("quit");

	xultb_guicore_system_init(&argc, argv);


	// show the window
//	struct xultb_window*win = xultb_window_create(&title);
//	win->width = 10;
//	SYNC_LOG(SYNC_VERB, "Window title:%s,width:%d,height:%d\n"
//			, win->title.str
//			, win->width, win->height);
//	xultb_guicore_platform_show(win);

	// show the list
	struct xultb_list*list = xultb_list_create(&title, &dc);
	xultb_str_t elem1 = xultb_str_create("good");
	xultb_str_t elem2 = xultb_str_create("very good");
	struct xultb_list_item*item = xultb_list_item_create_label(&elem1, NULL);
	opp_indexed_list_set(&list->_items, 0, item);
	list->win.show(&list->win);

	while(1) {
		usleep(100);
		xultb_guicore_walk(100);
	}
	return 0;
}

C_CAPSULE_END
