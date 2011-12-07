
#include "config.h"
#include "ui/xultb_guicore.h"
#include "ui/core/xultb_window.h"

C_CAPSULE_START

int main(int argc, char *argv[]) {

	xultb_str_t title = xultb_str_create("Test");
	xultb_str_t dc = xultb_str_create("quit");

	xultb_guicore_system_init(&argc, argv);
	xultb_list_create(&title, &dc);
	sleep(5);
	return 0;
}

C_CAPSULE_END
