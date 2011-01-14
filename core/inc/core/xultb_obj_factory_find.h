/*
 * xultb_obj_factory_find.h
 *
 *  Created on: Jan 14, 2011
 *      Author: ayaskanti
 */

#ifndef XULTB_OBJ_FACTORY_FIND_H_
#define XULTB_OBJ_FACTORY_FIND_H_

// find api
void*xultb_obj_find(struct xultb_obj_factory*obuff, int (*compare_func)(const void*data, const void*compare_data), const void*compare_data);
struct xultb_obj_factory*xultb_obj_find_list(struct xultb_obj_factory*obuff, int (*compare_func)(const void*data, const void*compare_data), const void*compare_data);
int xultb_obj_count(struct xultb_obj_factory*obuff);

// list api
void*xultb_obj_list_get(struct xultb_obj_factory*list, int token);


#endif /* XULTB_OBJ_FACTORY_FIND_H_ */
