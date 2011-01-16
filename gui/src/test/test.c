
#include "config.h"
#include "ui/xultb_guicore.h"


int main(void) {

	xultb_str_t title = xultb_str_create("Test");
	xultb_str_t dc = xultb_str_create("quit");

	xultb_guicore_system_init();
	xultb_list_create(&title, &dc);
	return 0;
}

