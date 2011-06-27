#ifndef XULTB_OBJ_FACTORY_UTILS_H
#define XULTB_OBJ_FACTORY_UTILS_H

typedef uint8_t SYNC_UWORD8_T;
typedef uint16_t SYNC_UWORD16_T;
typedef uint32_t SYNC_UWORD32_T;


#include "core/xultb_obj_factory_thread.h"
enum obj_log_type {
	OBJ_LOG_DEBUG,
	OBJ_LOG_ERROR,
};

/**
 *
 * This is an implementation of object based information
 * manipulation system. Please use "ob_" prefix in the
 * variable name. And remember to OBJ_FACTORY_UNREF()
 * that object.
 */

enum {
	OBJ_ITEM_FLAG_ALL = 1<<15,
};

enum {
	OBJ_FACTORY_HAS_LOCK = 1,
	OBJ_FACTORY_SWEEP_ON_UNREF = 1<<1,
	OBJ_FACTORY_HAS_HASH_PARTITION = 1<<2,
	OBJ_FACTORY_EXTENDED = 1<<3,
};

struct xultb_obj_pool;
//#define OBJ_HAS_HIJACK // use this if you need this
//#define OBJ_CAN_TEST_FLAG
#ifdef TEST_OBJ_FACTORY_UTILS
#define OBJ_HAS_HIJACK
#endif
//#ifndef SYNC_HAS_NO_THREAD
//#define OBJ_BUFFER_HAS_LOCK
//#endif
#define OBJ_HAS_WEAK // recommended
struct xultb_obj_factory {
	SYNC_UWORD16_T sign;
	SYNC_UWORD16_T pool_size;
	SYNC_UWORD16_T pool_count;
	SYNC_UWORD16_T use_count;
	SYNC_UWORD16_T slot_use_count;
	SYNC_UWORD16_T token_offset;
	SYNC_UWORD16_T obj_size;
	SYNC_UWORD16_T bitstring_size;
	SYNC_UWORD32_T memory_chunk_size;
	SYNC_UWORD8_T property;
#ifdef OBJ_BUFFER_HAS_LOCK
	xultb_mutex_t lock;
#endif
	int (*initialize)(void*data, const void*init_data, unsigned short size);
	int (*finalize)(void*data);
	int (*deep_copy)(void*data);
	void (*shrink)(void*data, unsigned short size);
	struct xultb_obj_pool*pools;
};

struct xultb_object_ext {
	SYNC_UWORD32_T hash;
	SYNC_UWORD16_T flag;
	SYNC_UWORD16_T token;
};

#define OBJ_FACTORY_CREATE(obuff, x, y, z, m) ({xultb_obj_factory_create_full(obuff, x, y, 1, OBJ_FACTORY_HAS_LOCK | OBJ_FACTORY_SWEEP_ON_UNREF, z, m, NULL, NULL);})
int xultb_obj_factory_create_full(struct xultb_obj_factory*obuff, int inc, SYNC_UWORD16_T obj_size, int token_offset, unsigned char property, int(*initialize)(void*data, const void*init_data, unsigned short size), int(*finalize)(void*data), int (*deep_copy)(void*data), void (*shrink)(void*data, unsigned short size));
void xultb_obj_factory_gc_donot_use(struct xultb_obj_factory*obuff);
void xultb_obj_factory_lock_donot_use(struct xultb_obj_factory*obuff);
void xultb_obj_factory_unlock_donot_use(struct xultb_obj_factory*obuff);
void xultb_obj_factory_destroy(struct xultb_obj_factory*obuff);

#if 0 // useful to check memory corruption
#define OBJ_FACTORY_CHECK_DEBUG(x) ({obj_factory_do_full(x, NULL, NULL, OBJ_ITEM_FLAG_ALL, 0, 0);})
#else
#define OBJ_FACTORY_CHECK_DEBUG(x)
#endif
#define OBJ_FACTORY_DO(x, y, z) ({obj_factory_do_full(x, y, z, OBJ_ITEM_FLAG_ALL, 0, 0);})
void xultb_obj_factory_do_full(struct xultb_obj_factory*obuff, int (*obj_do)(void*data, void*func_data), void*func_data, unsigned int if_flag, unsigned int if_not_flag, unsigned long hash);
void xultb_obj_factory_list_do_full(struct xultb_obj_factory*obuff, int (*obj_do)(void*data, void*func_data), void*func_data
		, unsigned int if_list_flag, unsigned int if_not_list_flag, unsigned int if_flag, unsigned int if_not_flag
		, unsigned long list_hash, unsigned long hash);
void xultb_obj_factory_verb(struct xultb_obj_factory*obuff, void (*verb_obj)(const void*data, const void*func_data), const void*func_data, void (*log)(void*log_data, const char*fmt, ...), void*log_data);

// obj api
#define OBJ_ALLOC1(x) ({obj_alloc3(x, 0, NULL);})
#define OBJ_ALLOC2(x, y) ({obj_alloc3(x, 0, y);})
void*xultb_obj_alloc3(struct xultb_obj_factory*obuff, SYNC_UWORD16_T size, const void*init_data);
void xultb_obj_shrink(void*data, int size);
void xultb_obj_set_hash(void*data, unsigned long hash);
void xultb_obj_set_flag(void*data, unsigned int flag);
void obj_unset_flag(void*data, unsigned int flag);
#ifdef OBJ_CAN_TEST_FLAG
int obj_test_flag(const void*data, unsigned int flag);
#endif
#define OBJ_FACTORY_REF(x) obj_ref(x, __FILE__, __LINE__)
void obj_ref(void*data, const char*filename, int lineno);
#define OBJ_FACTORY_UNREF(x) obj_unref((void**)&(x), __FILE__, __LINE__)
void obj_unref(void**data, const char*filename, int lineno);
//#ifdef OBJ_BUFFER_HAS_LOCK
#define OBJ_FACTORY_UNREF_UNLOCKED(x) ({obj_unref_unlocked((void**)&(x), __FILE__, __LINE__);})
void obj_unref_unlocked(void**data, const char*filename, int lineno);
//#endif
#ifdef OBJ_HAS_HIJACK
#define OBJ_FACTORY_HIJACK(x,y) ({obj_hijack((void**)&x, y, __FILE__, __LINE__);})
int obj_hijack(void**src, void*dest, const char*filename, int lineno);
#endif
// token api
void*obj_get(struct xultb_obj_factory*obuff, int token);
void obj_unset_flag_by_token(struct xultb_obj_factory*obuff, int token, unsigned int flag);
void obj_set_flag_by_token(struct xultb_obj_factory*obuff, int token, unsigned int flag);

// find api
#define OBJ_FIND_NOREF(x, y, z) ({obj_find_full(x, y, z, OBJ_ITEM_FLAG_ALL, 0, 0, 0);})
#define OBJ_FIND(x, y, z) ({obj_find_full(x, y, z, OBJ_ITEM_FLAG_ALL, 0, 0, 1);})
void*obj_find_full(struct xultb_obj_factory*obuff, int (*compare_func)(const void*data, const void*compare_data), const void*compare_data, unsigned int if_flag, unsigned int if_not_flag, unsigned long hash, unsigned int shouldref);
#define OBJ_FIND_LIST_DO_NOT_USE(x,y,z) ({obj_find_list_full_donot_use(x,y,z, OBJ_ITEM_FLAG_ALL, 0x00, 0, 1);})
void*obj_find_list_full_donot_use(struct xultb_obj_factory*obuff, int (*compare_func)(const void*data, const void*compare_data)
		, const void*compare_data, unsigned int if_flag, unsigned int if_not_flag, unsigned long hash, int shouldref);
int obj_count_donot_use(const struct xultb_obj_factory*obuff);
int obj_exists(struct xultb_obj_factory*obuff, const void*data);

// logger api
void xultb_obj_logger_set(void (*log_func)(enum obj_log_type ltype, char*format, ...));

#ifdef TEST_OBJ_FACTORY_UTILS
int obj_utils_test(void (*log)(void *log_data, const char*fmt, ...), void*log_data);
#endif

#endif // OBJ_FACTORY_UTILS_H
