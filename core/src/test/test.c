

#include <stdio.h>
#include "core/xultb_decorator.h"
#include "core/xultb_exttypes.h"



int main(void) {

	xultb_str_t str = {.str = "fine", .hash = 3, .len = 4};

	printf("String working [%s]\n", str.str);
	

	// TODO write test code
	return 0;
}

