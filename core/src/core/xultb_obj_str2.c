/*
 * obj_factory_list.c
 *
 *  Created on: Feb 9, 2011
 *      Author: root
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "sync/obj_factory_utils.h"
#include "sync/obj_str2.h"

enum {
#ifdef SYNC_LOW_MEMORY
	OBJ_STR2_BUFFER_INC = 32
#else
	OBJ_STR2_BUFFER_INC = 1024
#endif
};


static struct obj_factory str2_factory;
char*obj_str2_alloc(int size) {
	return xultb_obj_alloc3(&str2_factory, size, NULL);
}

char*obj_str2_reuse(char*string) {
	char *ret = NULL;
	int len;

	if(!string || !string[0]) {
		return NULL;
	}

	// try to find the string
	if(obj_exists(&str2_factory, string)) {
		OBJ_FACTORY_REF(string);
		return string;
	}

	len = strlen(string);
	ret = xultb_obj_alloc3(&str2_factory, len+1, NULL);

	if(!ret) {
		return NULL;
	}

	memcpy(ret, string, len);
	ret[len] = '\0';
	return ret;
}

void obj_str2_reuse2(char**dest, char*string) {
	int len;

	OBJ_FACTORY_UNREF(*dest);
	*dest = NULL;

	if(!string || !string[0]) {
		return;
	}
	// try to find the string
	if(obj_exists(&str2_factory, string)) {
		OBJ_FACTORY_REF(string);
		*dest = string;
		return;
	}
	len = strlen(string);
	*dest = xultb_obj_alloc3(&str2_factory, len+1, NULL);

	memcpy(*dest, string, len);
	*(*dest + len) = '\0';
}


char*obj_str2_dup(const char*string) {
	char *ret = NULL;
	int len;

	if(!string || !string[0]) {
		return NULL;
	}
	len = strlen(string);
	ret = xultb_obj_alloc3(&str2_factory, len+1, NULL);

	if(!ret) {
		return NULL;
	}

	memcpy(ret, string, len);
	ret[len] = '\0';
	return ret;
}

void obj_str2_dup2(char**dest, const char*string) {
	int len;

	OBJ_FACTORY_UNREF(*dest);
	*dest = NULL;

	if(!string || !string[0]) {
		return;
	}
	len = strlen(string);
	*dest = xultb_obj_alloc3(&str2_factory, len+1, NULL);

	memcpy(*dest, string, len);
	*(*dest + len) = '\0';
}


void obj_str2system_init() {
	OBJ_FACTORY_CREATE(&str2_factory, OBJ_STR2_BUFFER_INC, 8, NULL, NULL);
}

void obj_str2system_deinit() {
	xultb_obj_factory_destroy(&str2_factory);
}

#ifdef __cplusplus
}
#endif
