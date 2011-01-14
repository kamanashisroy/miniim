#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "core/xultb_obj_factory.h"
#include "core/xultb_obj_factory_find.h"

#define OBJ_HAS_WEAK
#define OBJ_HAS_TOKEN
#ifdef TEST_OBJ_FACTORY_UTILS
#define OBJ_DEBUG
#endif

#ifdef OBJ_DEBUG
#define CHECK_OBJ_SIGNATURE 0x24
#else
#undef CHECK_OBJ_SIGNATURE
#endif

#ifdef OBJ_BUFFER_HAS_LOCK
#define OBJ_BUFFER_LOCK(x) do { \
	while(xultb_mutex_trylock(&x->lock)) { \
		usleep(1); \
	} \
}while(0)
#endif

#define LIST_OBJ_POOL_SIZE 10
#define MAX_POOL_COUNT 100
struct obj_pool;
// bit hacks http://graphics.stanford.edu/~seander/bithacks.html

struct object {
	uint8_t refcount;
#ifdef CHECK_OBJ_SIGNATURE
	uint8_t signature;
#endif
	char*bitstring;
	uint8_t bit_index;
#ifdef OBJ_HAS_TOKEN
	int token;
#endif
	struct xultb_obj_factory*obuff;
};

#ifdef OBJ_DEBUG
static int test_use_count = 0;
#endif

struct xultb_obj_factory {
	int pool_size;
	size_t memory_chunk_size;
	size_t obj_size;
	size_t bitstring_size;
#ifdef OBJ_BUFFER_HAS_LOCK
	xultb_mutex_t lock;
#endif
#ifdef OBJ_HAS_WEAK
	struct object*weak_obj;
#endif
	int (*initialize)(void*data);
	int (*finalize)(void*data);
	int (*deep_copy)(void*data);
	int token_offset;
	struct obj_pool{
		char*bitstring; // each byte pair contains the usage and finalization flag respectively.
		char*head;
		char*end;
	} pools[MAX_POOL_COUNT];
};

struct obj_list_item {
	void*data;
};

static void (*do_log)(enum obj_log_type ltype, char*format, ...) = NULL;
static const struct obj_pool null_pool = {NULL,NULL,NULL};

struct xultb_obj_factory*xultb_obj_factory_create(int inc, size_t obj_size, int token_offset, int(*initialize)(void*data), int(*finalize)(void*data), int (*deep_copy)(void*data)) {
	struct xultb_obj_factory*obuff = NULL;

	obuff = malloc(sizeof(struct xultb_obj_factory));
	if(!obuff) {
		if(do_log)do_log(OBJ_LOG_ERROR ,"Out of memory\n");
		return NULL;
	}
	memset(obuff, 0, sizeof(struct xultb_obj_factory));
#ifdef OBJ_BUFFER_HAS_LOCK
	xultb_mutex_init(&obuff->lock);
#endif
	obuff->pool_size = inc;
	obuff->obj_size = obj_size + sizeof(struct object);
	obuff->initialize = initialize;
	obuff->finalize = finalize;
	obuff->bitstring_size = (inc >> 3) + ((inc > ((inc >> 3) << 3) )?1:0);
	obuff->memory_chunk_size = obuff->obj_size*inc + obuff->bitstring_size*2;
	obuff->token_offset = token_offset;
	obuff->deep_copy = deep_copy;

	return obuff;
}

void*xultb_obj_alloc(struct xultb_obj_factory*obuff) {
	char*ret = NULL;
	int i, pool_count = 0, k;
	struct object*obj;
	char*bitstring, *end, bit_index;
	uint8_t bsv;

#ifdef OBJ_BUFFER_HAS_LOCK
	OBJ_BUFFER_LOCK(obuff);
#endif
	do {
#ifdef OBJ_HAS_WEAK
		if(obuff->weak_obj && !obuff->weak_obj->refcount) {
			ret = (char*)obuff->weak_obj;
			obuff->weak_obj = NULL;
			break;
		}
#endif

		for(i=0;i<MAX_POOL_COUNT;i++) {
			if(!(bitstring = obuff->pools[i].bitstring)) {
				continue;
			}
			pool_count++;
			k = 0;
			end = obuff->pools[i].head;
			for(;bitstring < end;k++,bitstring+=2) {
				// find first 0
				bsv = (~(*bitstring));
				if(!bsv) continue;
#if 0
				bsv = bsv & (-bsv);
				bit_index = 0;
				while(bsv >>=1)bit_index++;
#else
#ifdef __GNUC__
				bit_index = __builtin_ctz(bsv); // number of leading 0 bits starting from lsb
#else
				#error "unknown compiler"
#endif
#endif
				
				//printf("%d\n", bit_index);
				if(((k<<3) + bit_index) < obuff->pool_size) {
					ret = obuff->pools[i].head + ((k<<3) + bit_index)*obuff->obj_size;
					obj = (struct object*)ret;
					obj->bit_index = bit_index;
					obj->bitstring = bitstring;
#ifdef OBJ_HAS_TOKEN
					obj->token = obuff->token_offset + i*obuff->pool_size + ((k<<3) + bit_index);
#endif
					obj->obuff = obuff;
					break;
				}
			}
			if(ret) break;
		}

		if(ret) {
			break;
		}

		if(pool_count == MAX_POOL_COUNT) {
			if(do_log)do_log(OBJ_LOG_ERROR, "Out of memory(exceeded max pool count)%s\n", ret - 1000);
			ret = NULL;
			break;
		}
		// allocate a block of memory
		ret = malloc(obuff->memory_chunk_size);
		if(!ret) {
			if(do_log)do_log(OBJ_LOG_ERROR, "Out of memory\n");
			ret = NULL;
			break;
		}
		// link
		for(i=0;i<MAX_POOL_COUNT;i++) {
			if(!obuff->pools[i].bitstring)
				break;
		}
		// clear memory
		memset(ret, 0, obuff->memory_chunk_size);

		// setup pool
		obuff->pools[i].bitstring = ret;
		obuff->pools[i].head = ret + (obuff->bitstring_size << 1);
		obuff->pools[i].end = ret + obuff->memory_chunk_size;

		ret = obuff->pools[i].head;
		obj = (struct object*)ret;
		obj->bit_index = 0;
		obj->bitstring = obuff->pools[i].bitstring;
#ifdef OBJ_HAS_TOKEN
		obj->token = obuff->token_offset + i*obuff->pool_size;
#endif
		obj->obuff = obuff;
	}while(0);

	do {
		if(!ret)
			break;

		obj = (struct object*)ret;
		ret = ret + sizeof(struct object);

		if(*(obj->bitstring+1) & ( 1 << obj->bit_index)) {
			if(obuff->finalize) {
				obuff->finalize(ret);
			}
		}

		if(obuff->initialize) {
			if(obuff->initialize(ret) != 0) {
				ret = NULL;
				break;
			}
		}

		obj->refcount++;
#ifdef CHECK_OBJ_SIGNATURE
		obj->signature = CHECK_OBJ_SIGNATURE;
#endif
		*(obj->bitstring) |= ( 1 << obj->bit_index);
		*(obj->bitstring+1) |= ( 1 << obj->bit_index); // say that the bit is initialized

#ifdef OBJ_DEBUG
		test_use_count++;
#endif

	} while(0);

#ifdef OBJ_BUFFER_HAS_LOCK
	xultb_mutex_unlock(&obuff->lock);
#endif
	return ret;
}

#ifdef OBJ_HAS_TOKEN
void*xultb_obj_get(struct xultb_obj_factory*obuff, int token) {
	int k, index;
	int pool_index;
	char*j;
	void*data = NULL;

#ifdef OBJ_BUFFER_HAS_LOCK
	OBJ_BUFFER_LOCK(obuff);
#endif
	do {
		if( (index = (token - obuff->token_offset)) < 0 ) break;
		k = index%obuff->pool_size;
		pool_index = (index - k)/obuff->pool_size;

		if(pool_index > MAX_POOL_COUNT) break;

		if(!obuff->pools[pool_index].bitstring) break;

		j = obuff->pools[pool_index].head + obuff->obj_size*k;
		if(((struct object*)j)->refcount) {
			data = (j+sizeof(struct object));
		}
	} while(0);
#ifdef OBJ_BUFFER_HAS_LOCK
	xultb_mutex_unlock(&obuff->lock);
#endif
	return data;
}

int xultb_obj_get_token(const void*data) {
	const struct object*obj = (data - sizeof(struct object));
#ifdef CHECK_OBJ_SIGNATURE
	assert(obj->signature == CHECK_OBJ_SIGNATURE);
#endif
	return obj->token;
}
#endif

void xultb_obj_ref(void*data) {
	struct object*obj = (data - sizeof(struct object));
#ifdef CHECK_OBJ_SIGNATURE
	assert(obj->signature == CHECK_OBJ_SIGNATURE);
#endif
	if(!obj->refcount) {
#ifdef OBJ_BUFFER_HAS_LOCK
		OBJ_BUFFER_LOCK(obj->obuff);
#endif
		//if(obj->obuff->weak_obj == obj)obj->obuff->weak_obj = NULL;
		*(obj->bitstring) |= ( 1 << obj->bit_index);
#ifdef OBJ_DEBUG
		// force the object initialized
		assert((*(obj->bitstring+1) & ( 1 << obj->bit_index)));
#endif
#ifdef OBJ_DEBUG
		test_use_count++;
#endif
#ifdef OBJ_BUFFER_HAS_LOCK
		xultb_mutex_unlock(&obj->obuff->lock);
#endif
	}
	obj->refcount++;
}

void xultb_obj_unref(void**data) {
	struct object*obj = (*data - sizeof(struct object));
#ifdef CHECK_OBJ_SIGNATURE
	assert(obj->signature == CHECK_OBJ_SIGNATURE);
#endif
#ifdef OBJ_DEBUG
	assert(obj->refcount);
#endif
	obj->refcount--;
	if(!obj->refcount) {
#ifdef OBJ_BUFFER_HAS_LOCK
	OBJ_BUFFER_LOCK(obj->obuff);
#endif
#ifdef OBJ_HAS_WEAK
		obj->obuff->weak_obj = obj;
#endif
		*(obj->bitstring) &= ~( 1 << obj->bit_index);
#ifdef OBJ_DEBUG
		test_use_count--;
#endif
#ifdef OBJ_BUFFER_HAS_LOCK
		xultb_mutex_unlock(&obj->obuff->lock);
#endif
	}
	*data = NULL;
	return;
}

int xultb_obj_hijack(void**src, void*dest) {
	struct object*obj = (*src - sizeof(struct object));
	int ret = -1;
#ifdef OBJ_BUFFER_HAS_LOCK
	OBJ_BUFFER_LOCK(obj->obuff);
#endif
	do {
#ifdef CHECK_OBJ_SIGNATURE
		assert(obj->signature == CHECK_OBJ_SIGNATURE);
#endif
#ifdef OBJ_DEBUG
		assert(obj->refcount);
#endif
		// see if we can do hijack
		if(obj->refcount != 1) {
			break;
		}

		// hijack
		memcpy(dest,*src,obj->obuff->obj_size - sizeof(struct object));
		obj->refcount--;
#ifdef OBJ_HAS_WEAK
		obj->obuff->weak_obj = obj;
#endif
		// clear flags
		*(obj->bitstring) &= ~( 1 << obj->bit_index);
		*(obj->bitstring+1) &= ~( 1 << obj->bit_index);
#ifdef OBJ_DEBUG
		test_use_count--;
#endif
		*src = dest;
		if(obj->obuff->deep_copy)
			obj->obuff->deep_copy(dest);
		ret = 0;
	} while(0);
#ifdef OBJ_BUFFER_HAS_LOCK
	xultb_mutex_unlock(&obj->obuff->lock);
#endif
	return ret;
}

void*xultb_obj_find(struct xultb_obj_factory*obuff, int (*compare_func)(const void*data, const void*compare_data), const void*compare_data) {
	int i, k;
	char*bitstring,*end,*data;
	char bsv;
	int bit_index;
	void *retval = NULL;
#ifdef OBJ_BUFFER_HAS_LOCK
	OBJ_BUFFER_LOCK(obuff);
#endif
	for(i=0;i<MAX_POOL_COUNT;i++) {
		if(!obuff->pools[i].bitstring) {
			continue;
		}

		// traverse the bitset
		k = 0;
		bitstring = obuff->pools[i].bitstring;
		end = obuff->pools[i].head;
		for(;bitstring < end;k++,bitstring+=2) {
			
			bsv = *bitstring;
			// get the bits to finalize
			while(bsv) {
#ifdef __GNUC__
				bit_index = __builtin_ctz(bsv); // number of leading 0 bits starting from lsb
#else
				#error "unknown compiler"
#endif
				if(((k<<3) + bit_index) < obuff->pool_size) {
					data = (obuff->pools[i].head + ((k<<3) + bit_index)*obuff->obj_size)+sizeof(struct object);
					if(compare_func(data, compare_data)) {
						retval = data;
						goto exit_point;
					}
					// clear
					bsv &= ~( 1 << bit_index);
				}
			}
		}
	}
	exit_point:
#ifdef OBJ_BUFFER_HAS_LOCK
	xultb_mutex_unlock(&obuff->lock);
#endif
	return data;
}

struct xultb_obj_factory*xultb_obj_find_list(struct xultb_obj_factory*obuff, int (*compare_func)(const void*data, const void*compare_data), const void*compare_data) {
	int i, k;
	char*bitstring,*end,*data;
	char bsv;
	int bit_index;
	struct obj_list_item*item = NULL;
	struct xultb_obj_factory*list = NULL;

#ifdef OBJ_BUFFER_HAS_LOCK
	OBJ_BUFFER_LOCK(obuff);
#endif
	for(i=0;i<MAX_POOL_COUNT;i++) {
		if(!obuff->pools[i].bitstring) {
			continue;
		}

		// traverse the bitset
		k = 0;
		bitstring = obuff->pools[i].bitstring;
		end = obuff->pools[i].head;
		for(;bitstring < end;k++,bitstring+=2) {
			
			bsv = *bitstring;
			// get the bits to finalize
			while(bsv) {
#ifdef __GNUC__
				bit_index = __builtin_ctz(bsv); // number of leading 0 bits starting from lsb
#else
				#error "unknown compiler"
#endif
				if(((k<<3) + bit_index) < obuff->pool_size) {
					data = (obuff->pools[i].head + ((k<<3) + bit_index)*obuff->obj_size)+sizeof(struct object);
					if(compare_func(data, compare_data)) {
						if(!list && !(list = xultb_obj_factory_create(LIST_OBJ_POOL_SIZE, sizeof(struct obj_list_item), 0, NULL, NULL, NULL))) {
							goto exit_point;
						}
						item = xultb_obj_alloc(list);
						if(item)
							item->data = data;
					}
					// clear
					bsv &= ~( 1 << bit_index);
				}
			}
		}
	}
	exit_point:
#ifdef OBJ_BUFFER_HAS_LOCK
	xultb_mutex_unlock(&obuff->lock);
#endif
	return list;
}

int xultb_obj_count(struct xultb_obj_factory*obuff) {
	int i, use_count = 0;
	char*j;
#ifdef OBJ_BUFFER_HAS_LOCK
	OBJ_BUFFER_LOCK(obuff);
#endif
	for(i=0;i<MAX_POOL_COUNT;i++) {
		if(obuff->pools[i].bitstring) {
			j = obuff->pools[i].head;
			for(;j<obuff->pools[i].end;j+=obuff->obj_size) {
				if(((struct object*)(j))->refcount) {
					use_count++;
				}
			}
		}
	}
#ifdef OBJ_BUFFER_HAS_LOCK
	xultb_mutex_unlock(&obuff->lock);
#endif
	return use_count;
}



void*xultb_obj_list_get(struct xultb_obj_factory*list, int token) {
	struct obj_list_item* item = xultb_obj_get(list, token);
	if(item)
		return item->data;
	return NULL;
}

#if 0
void*obj_dealloc(void*data) {
}
#endif

#if 0
void*obj_realloc(void*data) {
}
#endif

void xultb_obj_factory_gc(struct xultb_obj_factory*obuff) {
	int i, k, diff;
	char*bitstring,*end;
	char bsv;
	int use_count;
	int bit_index;
#ifdef CHECK_OBJ_SIGNATURE
	struct object*obj;
#endif
#ifdef OBJ_BUFFER_HAS_LOCK
	OBJ_BUFFER_LOCK(obuff);
#endif
	for(i=0;i<MAX_POOL_COUNT;i++) {
		if(!obuff->pools[i].bitstring) {
			continue;
		}

		use_count = 0;

		// traverse the bitset
		k = 0;
		bitstring = obuff->pools[i].bitstring;
		end = obuff->pools[i].head;
		for(;bitstring < end;k++,bitstring+=2) {

			use_count += __builtin_popcount((*bitstring) & 0xFF);
			// get the bits to finalize
			while((bsv = (~(*bitstring)) & (*(bitstring+1)))) {
#if 0
				bsv = bsv & (-bsv);
				bit_index = 0;
				while(bsv >>=1)bit_index++;
#else
#ifdef __GNUC__
				bit_index = __builtin_ctz(bsv); // number of leading 0 bits starting from lsb
#else
				#error "unknown compiler"
#endif
#endif
				if(((k<<3) + bit_index) < obuff->pool_size) {
					if(obuff->finalize) {
						obuff->finalize((obuff->pools[i].head + ((k<<3) + bit_index)*obuff->obj_size)+sizeof(struct object));
					}
					// clear
					*(bitstring+1) &= ~( 1 << bit_index);
				}
			}
		}

		// destroy memory chunk
		if(use_count == 0) {
#ifdef CHECK_OBJ_SIGNATURE
			obj = (struct object*)obuff->pools[i].head;
			assert(obj->signature == CHECK_OBJ_SIGNATURE);
#endif
#ifdef OBJ_HAS_WEAK
			if(obuff->weak_obj) {
				diff = ((char*)obuff->weak_obj) - obuff->pools[i].head;
				if(diff >= 0 && diff < obuff->memory_chunk_size) {
					obuff->weak_obj = NULL;
				}
			}
#endif
			free(obuff->pools[i].bitstring);
			obuff->pools[i] = null_pool;
		}
	}
#ifdef OBJ_BUFFER_HAS_LOCK
	xultb_mutex_unlock(&obuff->lock);
#endif
}

void xultb_obj_factory_do(struct xultb_obj_factory*obuff, void (*obj_do)(void*data, void*func_data), void*func_data) {
	int i;
	char*j;
#ifdef OBJ_BUFFER_HAS_LOCK
	OBJ_BUFFER_LOCK(obuff);
#endif
	for(i=0;i<MAX_POOL_COUNT;i++) {
		if(obuff->pools[i].bitstring) {
			j = obuff->pools[i].head;
			for(;j<obuff->pools[i].end;j+=obuff->obj_size) {
				if(((struct object*)(j))->refcount) {
					if(obj_do)obj_do(j + sizeof(struct object), func_data);
				}
			}
		}
	}
#ifdef OBJ_BUFFER_HAS_LOCK
	xultb_mutex_unlock(&obuff->lock);
#endif
}



void xultb_obj_factory_verb(struct xultb_obj_factory*obuff, void (*verb_obj)(const void*data, const void*func_data), const void*func_data) {
	int i, use_count = 0, pool_count = 0;
	char*j;
#ifdef OBJ_BUFFER_HAS_LOCK
	OBJ_BUFFER_LOCK(obuff);
#endif
	for(i=0;i<MAX_POOL_COUNT;i++) {
		if(obuff->pools[i].bitstring) {
			j = obuff->pools[i].head;
			for(;j<obuff->pools[i].end;j+=obuff->obj_size) {
				if(((struct object*)(j))->refcount) {
					if(verb_obj)verb_obj(j + sizeof(struct object), func_data);
					use_count++;
				}
			}
			pool_count++;
		}
	}
	if(do_log)do_log(OBJ_LOG_DEBUG, "pool count:%d, use count: %d, total memory: %d bytes, total used: %d bytes"
#ifdef OBJ_DEBUG
	",test use count :%d"
	", bitstring size :%d"
	", pool size :%d"
#endif
	"\n"
		, pool_count
		, use_count
		, (int)(pool_count*obuff->memory_chunk_size)
		, (int)(use_count*obuff->obj_size)
#ifdef OBJ_DEBUG
		, test_use_count
		, (int)obuff->bitstring_size
		, (int)obuff->pool_size
#endif
	);
#ifdef OBJ_BUFFER_HAS_LOCK
	xultb_mutex_unlock(&obuff->lock);
#endif
}

void xultb_obj_factory_destroy(struct xultb_obj_factory**data) {
	int i;
	char*bitstring,*end;
	struct xultb_obj_factory*obuff = *data;
#ifdef OBJ_BUFFER_HAS_LOCK
	OBJ_BUFFER_LOCK(obuff);
#endif
	for(i=0;i<MAX_POOL_COUNT;i++) {
		if(!obuff->pools[i].bitstring) {
			continue;
		}
		bitstring = obuff->pools[i].bitstring;
		end = obuff->pools[i].head;
		for(;bitstring < end;bitstring+=2) {
			*bitstring = 0;
		}
	}

#ifdef OBJ_BUFFER_HAS_LOCK
	xultb_mutex_unlock(&obuff->lock);
#endif
	xultb_obj_factory_gc(obuff);
#ifdef OBJ_BUFFER_HAS_LOCK
	xultb_mutex_destroy(&obuff->lock);
#endif
	free(obuff);
	*data = NULL;
}

void xultb_obj_logger_set(void (*log_func)(enum obj_log_type ltype, char*format, ...)) {
	do_log = log_func;
}

#ifdef TEST_OBJ_FACTORY_UTILS
struct pencil {
	int color;
	int depth;
};

static void pencil_verb(const void*data, const void*func_data) {
	const struct pencil* pen = data;
	if(do_log)do_log(OBJ_LOG_DEBUG, "Pen (color:%d,depth:%d)\n",pen->color,pen->depth);
}

static int pencil_compare(const void*data, const void*compare_data) {
	const int *color = compare_data;
	const struct pencil* pen = data;
	if(pen->color == *color) return 1;
	return 0;
}

static int pencil_compare_all(const void*data, const void*compare_data) {
	return 1;
}

static void pencil_do(void*data, void*func_data) {
	const struct pencil* pen = data;
	if(do_log)do_log(OBJ_LOG_DEBUG, "%sPen (color:%d,depth:%d)\n", (char*)func_data,pen->color,pen->depth);
}

static int obj_utils_test_helper(int inc) {
	struct xultb_obj_factory* bpencil = xultb_obj_factory_create(inc,sizeof(struct pencil), 0, NULL, NULL, NULL);
	struct pencil* pen;
	struct pencil hijacked;
	struct xultb_obj_factory*list = NULL;
	int color = 3;
	int index = 0;
	int depth_list[17] = {1, 2, 2, 3, 4, 5, 0, 7, 8, 9, 3, 4, 5, 6, 7, 8, 9};
	int ret = 0;

	// create 10 pencils
	for(index = 0; index < 10; index++) {
		pen = xultb_obj_alloc(bpencil);
		pen->color = (index%2)?3:1;
		pen->depth = index;
	}
	// remove 3
	pen = xultb_obj_get(bpencil, 0);
	XULTB_OBJ_UNREF(pen);
	pen = xultb_obj_get(bpencil, 1);
	XULTB_OBJ_HIJACK(pen,&hijacked); // hijack one
	pen = xultb_obj_get(bpencil, 6);
	XULTB_OBJ_UNREF(pen);
	if(inc%2) {
		xultb_obj_factory_gc(bpencil);
	}
	// create more 10
	for(index = 0; index < 10; index++) {
		pen = xultb_obj_alloc(bpencil);
		pen->color = (index%2)?3:1;
		pen->depth = index;
	}
	xultb_obj_factory_verb(bpencil, pencil_verb, NULL);

	if(!xultb_obj_find(bpencil, pencil_compare, &color)) {
		ret = -1;
	}

	if((list = xultb_obj_find_list(bpencil, pencil_compare_all, NULL))) {
		if(xultb_obj_count(list) != 17) {
			if(do_log)do_log(OBJ_LOG_ERROR, "TEST failed while finding all");
			ret = 1;
		}
		for(index = 0; (pen = xultb_obj_list_get(list,index)); index++) {
			if(pen->depth != depth_list[index]) {
				if(do_log)do_log(OBJ_LOG_ERROR, "TEST failed while matching depth\n");
				ret = -1;
			}
			if(do_log)do_log(OBJ_LOG_DEBUG, "There is a pencil in the list (color:%d,depth:%d)\n", pen->color, pen->depth);
		}
		xultb_obj_factory_destroy(&list);
	}
	if((list = xultb_obj_find_list(bpencil, pencil_compare, &color))) {
		if(xultb_obj_count(list) != 9) {
			if(do_log)do_log(OBJ_LOG_ERROR, "TEST failed while finding pencil color");
			ret = 1;
		}
		for(index = 0; (pen = xultb_obj_list_get(list,index)); index++) {
			if(pen->color != 3) {
				if(do_log)do_log(OBJ_LOG_ERROR, "TEST failed while checking pencil color\n");
				ret = -1;
			}
			if(do_log)do_log(OBJ_LOG_DEBUG, "There is a pencil in the list (color:%d,depth:%d)\n", pen->color, pen->depth);
		}
		xultb_obj_factory_destroy(&list);
	}

	xultb_obj_factory_do(bpencil, pencil_do, "[doing pencil command]");

	xultb_obj_factory_gc(bpencil);
	xultb_obj_factory_destroy(&bpencil);

	return ret;
}


int obj_utils_test(void) {
	int i;

	for(i=30;i>6;i--) {
		if(obj_utils_test_helper(i) != 0) {
			return -1;
		}
	}
	return 0;
}

#endif
