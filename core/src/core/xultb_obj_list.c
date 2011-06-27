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
#include "sync/obj_list.h"

static int obj_list_item_finalize(void*data) {
	struct obj_list_item*item = data;

	OBJ_FACTORY_UNREF(item->obj_data);
	return 0;
}

int obj_list_item_initialize(void*data, const void*obj_data, unsigned short notused) {
	struct obj_list_item*item = data;
	item->obj_data = (void*)obj_data;
	OBJ_FACTORY_REF(item->obj_data);
	return 0;
}

struct obj_list_item*obj_list_add_noref(struct obj_factory*olist, void*obj_data) {
	struct obj_list_item*item = OBJ_ALLOC2(olist, obj_data);
	// link
	if(!item) {
		return NULL;
	}

	OBJ_FACTORY_UNREF(obj_data);
	return item;
}

int obj_list_create2(struct obj_factory*olist, int pool_size, unsigned int flag) {
	return obj_factory_create_full(olist, pool_size
			, sizeof(struct obj_list_item)
			, 1, flag | OBJ_FACTORY_SWEEP_ON_UNREF
			, obj_list_item_initialize
			, obj_list_item_finalize, NULL, NULL);
}

struct obj_factory_find_list_helper {
	int (*compare_func)(const void*data, const void*compare_data);
	const void*compare_data;
	struct obj_factory*olist;
	int count;
};

static int obj_factory_list_compare(void*data, void*func_data) {
	struct obj_factory_find_list_helper*helper = func_data;

	if(helper->compare_func(data, helper->compare_data)) {
		return 0;
	}

	OBJ_ALLOC2(helper->olist, data);
	return 0;
}

int obj_list_find_from_factory(struct obj_factory*obuff, struct obj_factory*olist
		, int (*compare_func)(const void*data, const void*compare_data), const void*compare_data) {
	struct obj_factory_find_list_helper helper = {
			.compare_func = compare_func,
			.compare_data = compare_data,
			.olist = olist,
			.count = 0,
	};

	OBJ_FACTORY_DO(obuff, obj_factory_list_compare, &helper);
	return helper.count;
}


#ifdef __cplusplus
}
#endif
