#ifndef XULTB_EXTTYPES_H
#define XULTB_EXTTYPES_H

enum xultb_bool {
	TRUE = 1,
	FALSE = 0,
};

struct xultb_str {
	char*str;
	int hash;
	int len;
};

typedef struct xultb_str xultb_str_t;
typedef enum xultb_bool xultb_bool_t;

#endif
