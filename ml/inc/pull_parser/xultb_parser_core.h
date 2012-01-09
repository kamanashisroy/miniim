#ifndef XULTB_PULL_PARSER_CORE_H
#define XULTB_PULL_PARSER_CORE_H

#include "core/config.h"
#include "core/xultb_exttypes.h"
#include "opp/opp_factory.h"
#include "opp/opp_indexed_list.h"
#include "opp/opp_type.h"


C_CAPSULE_START

enum xultb_element_type {
	XULTB_ELEMENT_NODE = 1,
	XULTB_ELEMENT_TEXT,
};

struct xultb_ml_attr;

struct xultb_ml_elem {
	enum xultb_element_type type;
	xultb_str_t*content;
};

opp_vtable_declare(xultb_ml_node,
	xultb_str_t*(*get_name)(struct xultb_ml_node*elem);
//	int (*getChildCount)(struct xultb_ml_elem*elem);
	xultb_str_t*(*get_attribute_value)(struct xultb_ml_node*elem, const xultb_str_t*attr);
);

opp_class_declare(xultb_ml_node,
	struct xultb_ml_elem elem;
	xultb_str_t*name;
	struct opp_factory children;
	struct xultb_ml_attr*platform_data;
	void*parser;
);

//#define PARSER_LOG(...) SYNC_LOG(SYNC_VERB, __VA_ARGS__)
#define PARSER_LOG(...)

xultb_str_t*xultb_ml_get_attribute_value(struct xultb_ml_node*elem, const char*str);
xultb_str_t*xultb_ml_get_text(struct xultb_ml_node*elem);

struct xultb_ml_node*xultb_ml_get_node(struct xultb_ml_node*node, const char*str);
struct xultb_ml_node*xultb_ml_parse(char*src);
int xultb_parser_system_init();

C_CAPSULE_END

#endif // XULTB_PULL_PARSER_CORE_H
