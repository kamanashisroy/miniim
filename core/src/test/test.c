

#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include "core/xultb_decorator.h"
#include "core/xultb_exttypes.h"
#include "core/xultb_obj_factory.h"

static void test_log(enum obj_log_type ltype, char*format,...) {
	va_list vars;
	va_start(vars,format);
	if(ltype == OBJ_LOG_DEBUG) {
#if 0
		printf("--   ");
#else
		return;
#endif
	} else {
		printf("-- E ");
	}
	vprintf(format, vars);
	fflush(stdout);
	va_end(vars);
}



int main(void) {

	xultb_str_t str = {.str = "fine", .hash = 3, .len = 4};

	printf("String working [%s]\n", str.str);
	
	// TODO write test code
	xultb_obj_logger_set(test_log);
	if(obj_utils_test()) {
		printf("Error while testing object factory\n");
		return -1;
	}
	printf("Object factory test is successful\n");
	return 0;
}

