#include "core/config.h"
#include "core/xultb_exttypes.h"
#include "core/logger.h"
#include "opp/opp_factory.h"
#include "opp/opp_indexed_list.h"
#include "opp/opp_salt.h"
#include "pull_parser/xultb_parser_core.h"
#include "faxpp/parser.h"
#include "faxpp/entity_resolver.h"

C_CAPSULE_START


struct xultb_ml_attr {
	struct opp_object_ext ext;
	xultb_str_t*name;
	xultb_str_t*value;
	struct xultb_ml_attr*next;
};

static struct xultb_ml_attr*xultb_ml_new_attr(char*name, int name_len, char*value, int value_len);
static struct xultb_ml_node*xultb_ml_new_node(FAXPP_Parser *parser);
// ================== Parser Code ================================================================

static int xultb_ml_parse_node(struct xultb_ml_node*parent) {
	FAXPP_Parser *parser = (FAXPP_Parser *)parent->parser;
	int err = NO_ERROR,i;
	const FAXPP_Event*evt = FAXPP_get_current_event(parser);
	struct xultb_ml_attr root,*attrs;

	for(i = 0,attrs = &root; i < evt->attr_count; i++) {
		attrs->next = xultb_ml_new_attr(evt->attrs[i].name.ptr, evt->attrs[i].name.len, evt->attrs[i].value.value.ptr, evt->attrs[i].value.value.len);
		SYNC_ASSERT(attrs->next);
		attrs = attrs->next;
	}
	if(attrs != &root) {
		parent->platform_data = root.next;
	}

	if(evt->name.ptr) {
		parent->name = xultb_str_clone(evt->name.ptr, evt->name.len, 0);
	}
	i = 0;
	while((err = FAXPP_next_event(parser)) == 0) {
//      output_event(FAXPP_get_current_event(parser), stdout);

		evt = FAXPP_get_current_event(parser);
		if(evt->type == START_ELEMENT_EVENT) {
			struct xultb_ml_node*node = xultb_ml_new_node(parser);
			if(node) {
				opp_indexed_list_set(&parent->children,i,node);
				i++;
			}
		} else if(evt->type == CHARACTERS_EVENT && evt->value.ptr && !parent->elem.content) {
			parent->elem.content = xultb_str_clone(evt->value.ptr, evt->value.len, 0);
		} else if(evt->type == END_ELEMENT_EVENT || evt->type == SELF_CLOSING_ELEMENT_EVENT || evt->type == END_DOCUMENT_EVENT){
			return err;
		}
	}
	if(err != NO_ERROR) {
//      output_text(FAXPP_get_base_uri(parser), stdout);
		SYNC_LOG(SYNC_ERROR, ":%d:%d ERROR: %s\n", FAXPP_get_error_line(parser),
		FAXPP_get_error_column(parser), FAXPP_err_to_string(err));
	}
	return err;
}

static struct xultb_ml_node*xultb_ml_parse_impl(char*src) {
	FAXPP_Parser *parser = FAXPP_create_parser(WELL_FORMED_PARSE_MODE, FAXPP_utf8_transcoder);
	FAXPP_set_external_entity_callback(parser, entity_callback, 0);
	int err = 0;
    FILE*file = fopen(src, "r");
    if(file == 0) {
    	SYNC_LOG(SYNC_ERROR, "Open of '%s' failed:\n", src);
		FAXPP_free_parser(parser);
		return NULL;
    }

    err = FAXPP_init_parse_callback(parser, file_read_callback, file);
    if(err == NO_ERROR)
    	err = FAXPP_set_base_uri_str(parser, src);

    if(err != NO_ERROR) {
    	SYNC_LOG(SYNC_ERROR, "ERROR: %s\n", FAXPP_err_to_string(err));
    	FAXPP_free_parser(parser);
    	return NULL;
    }

    struct xultb_ml_node*node = NULL;
    while((err = FAXPP_next_event(parser)) == 0) {
    	const FAXPP_Event*evt = FAXPP_get_current_event(parser);
		if(!node && evt->type == START_ELEMENT_EVENT) {
			node = xultb_ml_new_node(parser);
		} else if(evt->type == END_ELEMENT_EVENT || evt->type == SELF_CLOSING_ELEMENT_EVENT || evt->type == END_DOCUMENT_EVENT){
			break;
		}
    }
    if(err != NO_ERROR) {
//      output_text(FAXPP_get_base_uri(parser), stdout);
    	SYNC_LOG(SYNC_ERROR, ":%d:%d ERROR: %s\n", FAXPP_get_error_line(parser),
        FAXPP_get_error_column(parser), FAXPP_err_to_string(err));
    }

    return node;
}



// ===============================================================================================




xultb_str_t*xultb_ml_get_attribute_value(struct xultb_ml_node*elem, const char*str) {
	const xultb_str_t name = xultb_str_create(str);
	return elem->vtable->get_attribute_value(elem, &name);
}

xultb_str_t*xultb_ml_get_text(struct xultb_ml_node*elem) {
	return elem->elem.content;
}

static struct opp_factory node_factory,attr_factory;

static xultb_str_t*xultb_get_attribute_value_impl(struct xultb_ml_node*elem, const xultb_str_t*attr) {
	struct xultb_ml_attr*obj = (struct xultb_ml_attr*)elem->platform_data;
	if(!obj || !attr || !attr->len) return NULL;
	do {
		if(obj->name->len == attr->len && !memcmp(obj->name->str, attr->str, attr->len)) {
			return obj->value;
		}
	}while((obj = obj->next));
	return NULL;
}

static xultb_str_t*xultb_get_name_impl(struct xultb_ml_node*elem) {
	return elem->name;
}

//opp_vtable_define(xultb_ml_node, );
struct opp_vtable_xultb_ml_node vtable_xultb_ml_node = {
	.get_attribute_value = xultb_get_attribute_value_impl,
	.get_name = xultb_get_name_impl,
};

static struct xultb_ml_attr*xultb_ml_new_attr(char*name, int name_len, char*value, int value_len) {
	struct xultb_ml_attr*attr;
	attr = (struct xultb_ml_attr*)opp_alloc4(&attr_factory, 0, 0, NULL);
	SYNC_ASSERT(attr);
	attr->name = xultb_str_alloc(name, name_len, NULL, 0);
	attr->value = xultb_str_alloc(value, value_len, NULL, 0);
	PARSER_LOG("attr name:[%1.1s], value:[%1.1s]\n", name, value);
	attr->next = NULL;
	return attr;
}

static struct xultb_ml_node*xultb_ml_new_node(FAXPP_Parser *parser) {
	struct xultb_ml_node*node;
	node = (struct xultb_ml_node*)opp_alloc4(&node_factory, 0, 0, NULL);
	SYNC_ASSERT(!opp_indexed_list_create2(&node->children, 4));
	node->elem.type = XULTB_ELEMENT_NODE;
	node->name = NULL;
	node->elem.content = NULL;
	node->platform_data = NULL;
	opp_vtable_set(node, xultb_ml_node);
	if(parser) {
		node->parser = parser;
		xultb_ml_parse_node(node);
	}
	return node;
}

struct xultb_ml_node*xultb_ml_get_node(struct xultb_ml_node*node, const char*str) {
	if(node->name && node->name->str && !strncmp(node->name->str, str, node->name->len)) {
		return node;
	}
	struct xultb_ml_node*obj,*ret = NULL;
	int i = 0;
	for(i=0;i>=0;i++) {
		opp_at_ncode2(obj, struct xultb_ml_node*, (&node->children), i,
//			SYNC_LOG(SYNC_VERB, "Comparing %s,%s,%d\n", obj->name->str, str, obj->name->len);
			if(obj->elem.type == XULTB_ELEMENT_NODE && obj->name && obj->name->str && !strncmp(obj->name->str, str, obj->name->len)) {
//				SYNC_LOG(SYNC_VERB, "Matched ..\n");
				ret = obj;
				i = -2;
			}
		) else {
			break;
		}
	}
	return ret;
}

struct xultb_ml_node*xultb_ml_parse(char*src) {
	return xultb_ml_parse_impl(src);
}

// TODO write destructor
// FAXPP_free_parser(parser);

int xultb_parser_system_init() {
	SYNC_ASSERT(!OPP_FACTORY_CREATE(
			&node_factory
			, 1,sizeof(struct xultb_ml_node)
			, NULL));
	SYNC_ASSERT(!OPP_FACTORY_CREATE(
			&attr_factory
			, 1,sizeof(struct xultb_ml_attr)
			, NULL));
	return 0;
}

C_CAPSULE_END

