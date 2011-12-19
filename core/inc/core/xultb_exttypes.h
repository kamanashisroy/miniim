#ifndef XULTB_EXTTYPES_H
#define XULTB_EXTTYPES_H

#include "core/config.h"
C_CAPSULE_START
enum {
	XULTB_TRUE = 1,
	XULTB_FALSE = 0,
};

struct xultb_str {
	char*str;
	int hash;
	int len;
};

typedef struct xultb_str xultb_str_t;
typedef int xultb_bool_t;

#define xultb_str_create(x) ({xultb_str_t y;y.str = x,y.hash = 0,y.len = strlen(x);y;})

#if 0
xultb_str_t*xultb_substring(xultb_str_t*src, int off, int width, xultb_str_t*dest);
#endif
#define xultb_substring(src,off,width,dest) ({(dest)->str = (src)->str+off;(dest)->len = width;(dest)->hash=0;dest;})

#define xultb_strcmp(x,y) ({int min = x->len>y->len?y->len:x->len;memcmp(x->str, y->str, min);})

#define xultb_equals_static(x,static_y) ({char static_text[] = static_y;(x->len == (sizeof(static_text)-1) && !strcmp(x->str, static_text));})

C_CAPSULE_END

#endif
