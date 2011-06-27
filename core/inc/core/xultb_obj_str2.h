/*
 * obj_string.h
 *
 *  Created on: Mar 2, 2011
 *      Author: root
 */

#ifndef OBJ_STR2_H_
#define OBJ_STR2_H_

#include "config.h"
#include "obj_factory_utils.h"

char*obj_str2_reuse(char*string);
void obj_str2_reuse2(char**dest, char*string);
char*obj_str2_dup(const char*string);
void obj_str2_dup2(char**dest, const char*string);
char*obj_str2_alloc(int size);

void obj_str2system_init();
void obj_str2system_deinit();

#endif /* OBJ_STR2_H_ */
