/*
 * obj_factory_list.h
 *
 *  Created on: Feb 9, 2011
 *      Author: Kamanashis Roy
 */

#ifndef OBJ_FACTORY_LIST_H_
#define OBJ_FACTORY_LIST_H_
#include "sync/config.h"
#include "sync/obj_factory_utils.h"

struct obj_list_item {
	struct xultb_object_ext _ext; // To access this try flag OBJ_FACTORY_EXTENDED
	void*obj_data;
};

struct obj_list_item*obj_list_add_noref(struct obj_factory*olist, void*obj_data);
#define OBJ_LIST_CREATE(olist, x) ({obj_list_create2(olist, x, OBJ_FACTORY_HAS_LOCK | OBJ_FACTORY_SWEEP_ON_UNREF);})
#define OBJ_LIST_CREATE_NOLOCK(olist, x) ({obj_list_create2(olist, x, OBJ_FACTORY_SWEEP_ON_UNREF);})
#define OBJ_LIST_CREATE_NOLOCK_EXT(olist, x) ({obj_list_create2(olist, x, OBJ_FACTORY_SWEEP_ON_UNREF | OBJ_FACTORY_EXTENDED);})
int obj_list_create2(struct obj_factory*olist, int pool_size, unsigned int flag);
int obj_list_find_from_factory(struct obj_factory*obuff, struct obj_factory*olist, int (*compare_func)(const void*data, const void*compare_data), const void*compare_data);

#endif /* OBJ_FACTORY_LIST_H_ */
