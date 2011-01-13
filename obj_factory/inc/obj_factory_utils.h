#ifndef OBJ_FACTORY_UTILS_H
#define OBJ_FACTORY_UTILS_H

#include "obj_factory_thread.h"
enum obj_log_type {
	OBJ_LOG_DEBUG,
	OBJ_LOG_ERROR,
};

struct obj_factory;
struct obj_factory*obj_factory_create(int inc, size_t obj_size, int token_offset, int(*initialize)(void*data), int(*finalize)(void*data), int (*deep_copy)(void*data));
void obj_factory_gc(struct obj_factory*obuff);
void obj_factory_destroy(struct obj_factory**obuff);
void obj_factory_do(struct obj_factory*obuff, void (*obj_do)(void*data, void*func_data), void*func_data);
void obj_factory_verb(struct obj_factory*obuff, void (*verb_obj)(const void*data, const void*func_data), const void*func_data);

// obj api
void*obj_alloc(struct obj_factory*obuff);
void obj_ref(void*data);
#define OBJ_UNREF(x) ({obj_unref((void**)&x);})
void obj_unref(void**data);
#define OBJ_HIJACK(x,y) ({obj_hijack((void**)&x,y);})
int obj_hijack(void**src, void*dest);

// token api
int obj_get_token(const void*data);
void*obj_get(struct obj_factory*obuff, int token);

// find api
void*obj_find(struct obj_factory*obuff, int (*compare_func)(const void*data, const void*compare_data), const void*compare_data);
struct obj_factory*obj_find_list(struct obj_factory*obuff, int (*compare_func)(const void*data, const void*compare_data), const void*compare_data);
int obj_count(struct obj_factory*obuff);

// list api
void*obj_list_get(struct obj_factory*list, int token);

// logger api
void obj_logger_set(void (*log_func)(enum obj_log_type ltype, char*format, ...));


#ifdef TEST_OBJ_FACTORY_UTILS
int obj_utils_test(void);
#endif

#endif // OBJ_FACTORY_UTILS_H
