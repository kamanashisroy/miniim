#ifndef XULTB_OBJ_FACTORY_UTILS_H
#define XULTB_OBJ_FACTORY_UTILS_H

#include "core/xultb_obj_factory_thread.h"
enum obj_log_type {
	OBJ_LOG_DEBUG,
	OBJ_LOG_ERROR,
};

struct xultb_obj_factory;
struct xultb_obj_factory*xultb_obj_factory_create(int inc, size_t obj_size, int token_offset, int(*initialize)(void*data), int(*finalize)(void*data), int (*deep_copy)(void*data));
void xultb_obj_factory_gc(struct xultb_obj_factory*obuff);
void xultb_obj_factory_destroy(struct xultb_obj_factory**obuff);
void xultb_obj_factory_do(struct xultb_obj_factory*obuff, void (*obj_do)(void*data, void*func_data), void*func_data);
void xultb_obj_factory_verb(struct xultb_obj_factory*obuff, void (*verb_obj)(const void*data, const void*func_data), const void*func_data);

// obj api
void*xultb_obj_alloc(struct xultb_obj_factory*obuff);
void xultb_obj_ref(void*data);
#define XULTB_OBJ_UNREF(x) ({xultb_obj_unref((void**)&x);})
void xultb_obj_unref(void**data);
#define XULTB_OBJ_HIJACK(x,y) ({xultb_obj_hijack((void**)&x,y);})
int xultb_obj_hijack(void**src, void*dest);

// token api
int xultb_obj_get_token(const void*data);
void*xultb_obj_get(struct xultb_obj_factory*obuff, int token);

// logger api
void xultb_obj_logger_set(void (*log_func)(enum obj_log_type ltype, char*format, ...));


#ifdef TEST_OBJ_FACTORY_UTILS
int obj_utils_test(void);
#endif

#endif // XULTB_OBJ_FACTORY_UTILS_H
