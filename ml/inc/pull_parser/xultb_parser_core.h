#ifndef XULTB_PULL_PARSER_CORE_H
#define XULTB_PULL_PARSER_CORE_H

#include "config.h"
#include "core/xultb_exttypes.h"
#include "opp/opp_factory.h"
#include "opp/opp_indexed_list.h"

C_CAPSULE_START

enum xultb_element_type {
	XULTB_ELEMENT_NODE = 0,
	XULTB_ELEMENT_TEXT,
};

struct xultb_ml_attr {
	struct opp_object_ext ext;
	xultb_str_t*name;
	xultb_str_t*value;
};

struct xultb_ml_elem {
	enum xultb_element_type type;
	xultb_str_t*content;
};

struct xultb_ml_node {
	struct xultb_ml_elem elem;
	xultb_str_t*(*get_name)(struct xultb_ml_node*elem);
//	int (*getChildCount)(struct xultb_ml_elem*elem);
	xultb_str_t*(*get_attribute_value)(struct xultb_ml_node*elem, xultb_str_t*attr);
	struct opp_factory children;
	struct opp_factory attributes;
};

C_CAPSULE_END

#endif // XULTB_PULL_PARSER_CORE_H
