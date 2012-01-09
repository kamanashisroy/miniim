/*
 * xultb_string.c
 *
 *  Created on: Dec 21, 2011
 *      Author: ayaskanti
 */

#include "opp/opp_factory.h"
#include "core/xultb_string.h"

xultb_str_t*DOT;
xultb_str_t*BLANK_STRING;
xultb_str_t*ASTERISKS_STRING;
static struct opp_factory xultb2_factory;
xultb_str_t*xultb_str_alloc(char*content, int len, xultb_str_t*proto, int scalability_index) {
	if(content) {
		struct xultb_str*str = opp_alloc4(&xultb2_factory, 0, 0, NULL);
		XULTB_ASSERT_RETURN(str, NULL);
		str->size = str->len = len;
		str->str = content;
		str->hash = 0;
		str->proto = proto?OPPREF(proto):NULL;
		return str;
	} else {
		struct xultb_str*str = opp_alloc4(&xultb2_factory, sizeof(struct xultb_str)+len+1, 0, NULL);
		XULTB_ASSERT_RETURN(str, NULL);
		str->str = (char*)(str+1);
		*str->str = '\0';
		str->len = 0;
		str->size = len+1;
		str->hash = 0;
		str->proto = NULL;
		return str;
	}
}

xultb_str_t*xultb_str_clone(const char*content, int len, int scalability_index) {
	XULTB_ASSERT_RETURN(content && len, NULL);
	struct xultb_str*str = opp_alloc4(&xultb2_factory, sizeof(struct xultb_str)+len+1, 0, NULL);
	XULTB_ASSERT_RETURN(str, NULL);
	str->size = len+1;
	str->len = len;
	str->str = (char*)(str+1);
	if(len) {
		memcpy(str->str, content, len);
	}
	*(str->str+len) = '\0';
	str->hash = 0;
	str->proto = NULL;
	return str;
}

xultb_str_t*xultb_str_trim(xultb_str_t*text) {
	// TODO trim
	return text;
}


xultb_str_t*xultb_str_cat(xultb_str_t*text, xultb_str_t*suffix) {
	// TODO check if we have enough space
	SYNC_ASSERT(text->size > (text->len + suffix->len));
	memcpy(text->str+text->len, suffix->str, suffix->len);
	text->len += suffix->len;
	*(text->str+text->len) = '\0';
	return text;
}

xultb_str_t*xultb_str_cat_char(xultb_str_t*text, char c) {
	SYNC_ASSERT(text->size > (text->len + 1));
	*(text->str+text->len) = c;
	text->len++;
	*(text->str+text->len) = '\0';
	return text;
}

xultb_str_t*xultb_str_cat_static(xultb_str_t*text, char*suffix) {
	int len = strlen(suffix);
	SYNC_ASSERT(text->size > (text->len + len));
	memcpy(text->str+text->len, suffix, len);
	text->len += len;
	*(text->str+text->len) = '\0';
	return text;
}

xultb_str_t*xultb_str_set_len(xultb_str_t*text, int len) {
	SYNC_ASSERT(text->size > len);
	text->len = len;
	*(text->str+len) = '\0';
	return text;
}

OPP_CB(xultb_str) {
	xultb_str_t*string = (xultb_str_t*)data;
	switch(callback) {
	case OPPN_ACTION_INITIALIZE:
		return 0;
	case OPPN_ACTION_FINALIZE:
		OPPUNREF(string->proto);
		break;
	}
	return 0;
}

void xultb_str_system_init() {
	SYNC_ASSERT(!OPP_FACTORY_CREATE(&xultb2_factory, 128, sizeof(struct xultb_str)+32, OPP_CB_FUNC(xultb_str)));
	BLANK_STRING = xultb_str_alloc("", 0, NULL, 0);
	ASTERISKS_STRING = xultb_str_alloc("***********************************************", 30, NULL, 0);
	DOT = xultb_str_alloc(".", 1, NULL, 0);
}

void xultb_str_system_deinit() {
	opp_factory_destroy(&xultb2_factory);
}
