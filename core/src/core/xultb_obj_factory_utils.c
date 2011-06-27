#ifdef __cplusplus
{
#endif

#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "core/xultb_obj_factory.h"
#include "core/xultb_obj_factory_find.h"
#include "core/xultb_obj_queue.h"
#include "core/xultb_obj_list.h"

// TODO fix the slot bug, write test code to create the slot problem
#define SYNC_BIT32

#ifdef SYNC_BIT32
enum {
	BITSTRING_MASK = 0xFFFFFFFF
};
#define BITSTRING_TYPE SYNC_UWORD32_T
#define OBJ_NORMALIZE_SIZE(x) ({(x+3)&~3;})
#define BIT_PER_STRING 32
#define BITSTRING_IDX_TO_BITS(x) (x<<5)
#endif

#ifndef SYNC_ASSERT
#define SYNC_ASSERT(x) assert(x)
#endif

#define AUTO_GC	// recommended
#define OPTIMIZE_OBJ_LOOP // recommended
#define OBJ_HAS_TOKEN // recommended
#ifdef TEST_OBJ_FACTORY_UTILS
#define OBJ_DEBUG
#endif
#define HASH_PARTITION // recommended if you use hash

#ifdef OBJ_DEBUG
#define OBJ_DEBUG_REFCOUNT 4 // only for testing
enum {
	OBJ_TRACE_SIZE = 4,
};
#endif

#ifdef OBJ_DEBUG
#define FACTORY_OBJ_SIGN 0x24
#else
#undef FACTORY_OBJ_SIGN
#endif

#ifdef OBJ_BUFFER_HAS_LOCK
#define OBJ_BUFFER_LOCK(x) do { \
	while(((x)->property & OBJ_FACTORY_HAS_LOCK) && xultb_mutex_trylock(&(x)->lock)) { \
		sync_usleep(1); \
	} \
}while(0)

#define OBJ_BUFFER_UNLOCK(x) do { \
	if((x)->property & OBJ_FACTORY_HAS_LOCK) { \
		xultb_mutex_unlock(&(x)->lock); \
	} \
}while(0)
#else
#define OBJ_BUFFER_LOCK(x)
#define OBJ_BUFFER_UNLOCK(x)
#endif

#define CHECK_POOL(x) ({SYNC_ASSERT((x)->end > (x)->head \
	&& (x)->head > ((SYNC_UWORD8_T*)(x)->bitstring) \
	&& ((SYNC_UWORD8_T*)(x)->bitstring) > ((SYNC_UWORD8_T*)(x)) \
	&& (!(x)->weak_obj || ((SYNC_UWORD8_T*)(x)->weak_obj) < (x)->end) \
	&& (((BITSTRING_TYPE*)((x)+1)) == (x)->bitstring));})

#ifdef OBJ_DEBUG
#define CHECK_OBJ(x) ({SYNC_ASSERT((x)->signature == FACTORY_OBJ_SIGN);})
#else
#define CHECK_OBJ(x)
#endif

#ifdef AUTO_GC
#define DO_AUTO_GC_CHECK(x) do{\
	if((x)->pool_count*(x)->pool_size - (x)->slot_use_count > ((x)->pool_size << 1)) { \
		obj_factory_gc_nolock(x); \
	} \
}while(0);
#else
#define DO_AUTO_GC_CHECK(x)
#endif

struct xultb_obj_pool{
	SYNC_UWORD16_T idx;
	SYNC_UWORD16_T filler;
#ifdef OBJ_HAS_WEAK
	struct sync_object*weak_obj;
#endif
	BITSTRING_TYPE*bitstring; // each word(doublebyte) pair contains the usage and finalization flag respectively.
	SYNC_UWORD8_T*head;
	SYNC_UWORD8_T*end;
	struct xultb_obj_pool*next;
};

// bit hacks http://graphics.stanford.edu/~seander/bithacks.html

extern struct syncsystem_config syncsystem_conf;

enum {
	BITFIELD_FINALIZE = 1,
	BITFIELD_PAIRED = 2,
#ifndef OBJ_NO_FLAG_OPTIMIZATION
	BITFIELD_PERFORMANCE = 3,
	BITFIELD_PERFORMANCE1 = 4,
	BITFIELD_PERFORMANCE2 = 5,
	BITFIELD_PERFORMANCE3 = 6,
	BITFIELD_SIZE = 7,
#else
	BITFIELD_SIZE = 3,
#endif
};

#ifndef OBJ_NO_FLAG_OPTIMIZATION
enum {
	OBJ_ITEM_FLAG_PERFORMANCE = 1,
	OBJ_ITEM_FLAG_PERFORMANCE1 = 1<<1,
	OBJ_ITEM_FLAG_PERFORMANCE2 = 1<<2,
	OBJ_ITEM_FLAG_PERFORMANCE3 = 1<<3,
};
#endif

enum {
	OBJ_FACTORY_INITIALIZED = 0x3428,
};

struct sync_object {
	SYNC_UWORD16_T refcount;
	SYNC_UWORD8_T bit_idx;
	SYNC_UWORD8_T slots;
#ifdef FACTORY_OBJ_SIGN
	SYNC_UWORD32_T signature;
#endif
	BITSTRING_TYPE*bitstring;
	struct xultb_obj_factory*obuff;
#ifdef OBJ_DEBUG_REFCOUNT
	struct {
		const char *filename;
		SYNC_UWORD16_T lineno;
		SYNC_UWORD16_T refcount;
		char op;
		char filler[3];
	}ref_trace[OBJ_TRACE_SIZE];
	SYNC_UWORD32_T rt_idx;
#endif
};

#define SET_PAIRED_BITS(x) do{\
	SYNC_ASSERT((x)->slots <= BIT_PER_STRING); \
	if((x)->slots>1) { \
		*((x)->bitstring+BITFIELD_PAIRED) |= ((1<<((x)->slots))-1)<<((x)->bit_idx+1); \
	} \
	SYNC_ASSERT(!(*((x)->bitstring) & *((x)->bitstring+BITFIELD_PAIRED))); \
	(x)->obuff->slot_use_count += (x)->slots; \
}while(0);

#define UNSET_PAIRED_BITS(x) do{\
	SYNC_ASSERT((x)->slots <= BIT_PER_STRING); \
	/*SYNC_ASSERT(bitfield == (bitfield & *((x)->bitstring+BITFIELD_PAIRED)));*/ \
	if((x)->slots>1) { \
		*((x)->bitstring+BITFIELD_PAIRED) &= ~(((1<<((x)->slots))-1)<<((x)->bit_idx+1)); \
	} \
	(x)->obuff->slot_use_count -= (x)->slots; \
	(x)->slots = 0; \
}while(0);

static void (*do_log)(enum obj_log_type ltype, char*format, ...) = NULL;

static void obj_factory_gc_nolock(struct xultb_obj_factory*obuff);

int xultb_obj_factory_create_full(struct xultb_obj_factory*obuff, int inc, SYNC_UWORD16_T obj_size, int token_offset, unsigned char property, int(*initialize)(void*data, const void*init_data, unsigned short size), int(*finalize)(void*data), int (*deep_copy)(void*data), void (*shrink)(void*data, unsigned short size)) {
	SYNC_ASSERT(obj_size < (1024<<1));
#ifdef SYNC_LOW_MEMORY
	SYNC_ASSERT(inc < 1024);
#else
	SYNC_ASSERT(inc < (1024<<3));
#endif
	if(obuff->sign == OBJ_FACTORY_INITIALIZED) {
		if(do_log)do_log(OBJ_LOG_ERROR, "obj is already initiated\n");
		return 0;
	}
	obuff->sign = OBJ_FACTORY_INITIALIZED;
#ifdef OBJ_BUFFER_HAS_LOCK
	if(property & OBJ_FACTORY_HAS_LOCK) {
		xultb_mutex_init(&obuff->lock);
	}
#endif
	obuff->property = property | OBJ_FACTORY_SWEEP_ON_UNREF; // force sweep
	obuff->pool_size = inc;
	obj_size = OBJ_NORMALIZE_SIZE(obj_size);
	obuff->obj_size = obj_size + sizeof(struct sync_object);
	obuff->initialize = initialize;
	obuff->finalize = finalize;
	obuff->deep_copy = deep_copy;
	obuff->shrink = shrink;
//	obuff->bitstring_size = (inc >> 3) + ((inc > ((inc >> 3) << 3) )?1:0);
	obuff->bitstring_size = (inc+7) >> 3;
	obuff->bitstring_size = OBJ_NORMALIZE_SIZE(obuff->bitstring_size);
	obuff->bitstring_size = obuff->bitstring_size*BITFIELD_SIZE;
	obuff->memory_chunk_size = obuff->obj_size*inc + obuff->bitstring_size;
	obuff->token_offset = token_offset;

	obuff->pool_count = 0;
	obuff->use_count = 0;
	obuff->slot_use_count = 0;
	obuff->pools = NULL;

#ifndef OBJ_MAX_BUFFER_SIZE
#define OBJ_MAX_BUFFER_SIZE (4096<<10)
#endif
	SYNC_ASSERT(obuff->memory_chunk_size < OBJ_MAX_BUFFER_SIZE);

	return 0;
}

#if 0
#define SYNC_OBJ_CTZ(x) 
#else
#define SYNC_OBJ_POPCOUNT(x) __builtin_popcount(x)
#define SYNC_OBJ_CTZ(x) __builtin_ctz(x)
#endif

void*xultb_obj_alloc3(struct xultb_obj_factory*obuff, SYNC_UWORD16_T size, const void*init_data) {
	SYNC_UWORD8_T*ret = NULL;
	int k;
	struct sync_object*obj;
	BITSTRING_TYPE*bitstring,bsv,mask;
	SYNC_UWORD8_T bit_idx;
	SYNC_UWORD16_T obj_idx;
	SYNC_UWORD8_T slots = 1;
#ifdef OBJ_DEBUG
	int loop_breaker = 0;
#endif
	struct xultb_obj_pool*pool, *addpoint;
	
	SYNC_ASSERT(obuff->sign == OBJ_FACTORY_INITIALIZED);

	OBJ_BUFFER_LOCK(obuff);
	do {
		if(size) {
			OBJ_NORMALIZE_SIZE(size);
			size += sizeof(struct sync_object);
			slots = size / obuff->obj_size + ((size % obuff->obj_size)?1:0);
			if(slots > BIT_PER_STRING) {
				if(do_log)do_log(OBJ_LOG_ERROR, "Too big allocation request\n");
				break;
			}
		}
#ifdef OBJ_HAS_WEAK
		if(slots == 1) {
			for(pool = obuff->pools;pool; pool = pool->next) {
				CHECK_POOL(pool);
				if(pool->weak_obj) {
					SYNC_ASSERT(!pool->weak_obj->refcount);
					ret = (SYNC_UWORD8_T*)pool->weak_obj;
					pool->weak_obj = NULL;
					break;
				}
			}
		}
		if(ret) {
			break;
		}
#endif
		for(addpoint = NULL, pool = obuff->pools;pool;pool = pool->next) {
			k = 0;
			if(!addpoint && (!pool->next || (pool->idx+1 != pool->next->idx))) {
				addpoint = pool;
			}
			bitstring = pool->bitstring;
			for(;BITSTRING_IDX_TO_BITS(k) < obuff->pool_size;k++,bitstring+=BITFIELD_SIZE) {
				// find first 0
				bsv = (~(*bitstring | *(bitstring+BITFIELD_PAIRED)));
				if(!bsv) continue;
#ifdef OBJ_DEBUG
				loop_breaker = 0;
#endif
				while(bsv) {
#ifdef OBJ_DEBUG
					loop_breaker++;
					SYNC_ASSERT(loop_breaker < BIT_PER_STRING);
#endif
					if(slots > 1 && SYNC_OBJ_POPCOUNT(bsv) < slots) {
						break;
					}
					bit_idx = SYNC_OBJ_CTZ(bsv);
					if(slots > 1 && bit_idx + slots > BIT_PER_STRING) {
						// we cannot do it
						break;
					}
					
#if 0
					for(j=1;j<slots;j++) {
						if(!(bsv & (1<<(bit_idx+j)))) {
							j = 44;
							break;
						}
					}
					if(j == 44) {
						bsv &= ~(1<<bit_idx);
						continue;
					}
#else
					mask = ((1 << (slots+1))-1)<<bit_idx;
					if((mask & bsv) != mask) {
						bsv &= ~mask;
						continue;
					}
#endif
					obj_idx = BITSTRING_IDX_TO_BITS(k) + bit_idx;
					//printf("%d\n", bit_idx);
					if(obj_idx < obuff->pool_size) {
						ret = pool->head + obj_idx*obuff->obj_size;
						obj = (struct sync_object*)ret;
						obj->bit_idx = bit_idx;
						obj->bitstring = bitstring;
#ifdef OBJ_HAS_TOKEN
						if(obuff->property & OBJ_FACTORY_EXTENDED) {
							((struct xultb_object_ext*)(obj+1))->token = obuff->token_offset + pool->idx*obuff->pool_size + obj_idx;
						}
#endif
						obj->obuff = obuff;
						CHECK_POOL(pool);
					}
					break;
				}
				if(ret) {
					break;
				}
			}
			if(ret) break;
		}

		if(ret) {
#ifdef OBJ_HAS_WEAK
			pool->weak_obj = NULL;
#endif
			break;
		}

		// allocate a block of memory
		ret = malloc(sizeof(struct xultb_obj_pool) + obuff->memory_chunk_size);
		if(!ret) {
			if(do_log)do_log(OBJ_LOG_ERROR, "Out of memory\n");
			ret = NULL;
			break;
		}
		pool = (struct xultb_obj_pool*)ret;
		ret += sizeof(struct xultb_obj_pool);

		// clear memory
		memset(ret, 0, obuff->bitstring_size);
		if(!obuff->pools) {
			obuff->pools = pool;
			pool->idx = 0;
		} else {
			SYNC_ASSERT(addpoint);
			// insert the pool in appropriate place
			pool->next = addpoint->next;
			addpoint->next = pool;
			pool->idx = addpoint->idx+1;
		}
		SYNC_ASSERT(pool->idx < 512);

		obuff->pool_count++;

		// setup pool
		pool->bitstring = (BITSTRING_TYPE*)ret;
#ifdef OBJ_HAS_WEAK
		pool->weak_obj = NULL;
#endif
		pool->head = ret + (obuff->bitstring_size);
		pool->end = ret + obuff->memory_chunk_size;
		pool->next = NULL;
		CHECK_POOL(pool);
		
		ret = pool->head;
		obj = (struct sync_object*)ret;
		obj->bit_idx = 0;
		obj->bitstring = pool->bitstring;
#ifdef OBJ_HAS_TOKEN
		if(obuff->property & OBJ_FACTORY_EXTENDED) {
			((struct xultb_object_ext*)(obj+1))->token = obuff->token_offset + pool->idx*obuff->pool_size;
		}
#endif
		obj->obuff = obuff;
	}while(0);

	do {
		if(!ret)
			break;

		obj = (struct sync_object*)ret;
		ret = (SYNC_UWORD8_T*)(obj+1);

		if(*(obj->bitstring+BITFIELD_FINALIZE) & ( 1 << obj->bit_idx)) {
			if(obuff->finalize) {
				obuff->finalize(ret);
			}
		}
#ifdef FACTORY_OBJ_SIGN
		obj->signature = FACTORY_OBJ_SIGN;
#endif
		if(obuff->property & OBJ_FACTORY_EXTENDED) {
			((struct xultb_object_ext*)(obj+1))->flag = OBJ_ITEM_FLAG_ALL;
			((struct xultb_object_ext*)(obj+1))->hash = 0;
		}
		obj->refcount = 1;
		obj->slots = slots;

		*(obj->bitstring) |= ( 1 << obj->bit_idx);
		*(obj->bitstring+BITFIELD_FINALIZE) |= ( 1 << obj->bit_idx);
#ifndef OBJ_NO_FLAG_OPTIMIZATION
		*(obj->bitstring+BITFIELD_PERFORMANCE) &= ~( 1 << obj->bit_idx);
		*(obj->bitstring+BITFIELD_PERFORMANCE1) &= ~( 1 << obj->bit_idx);
		*(obj->bitstring+BITFIELD_PERFORMANCE2) &= ~( 1 << obj->bit_idx);
		*(obj->bitstring+BITFIELD_PERFORMANCE3) &= ~( 1 << obj->bit_idx);
#endif
		obuff->use_count++;
#ifdef OBJ_DEBUG_REFCOUNT
		obj->rt_idx = 0;
#endif

		SET_PAIRED_BITS(obj);
		if(obuff->initialize) {
			if(obuff->initialize(ret, init_data, obj->slots*obuff->obj_size - sizeof(struct sync_object))) {
				ret = NULL;
				obj->refcount = 0;
				*(obj->bitstring) &= ~( 1 << obj->bit_idx);
				*(obj->bitstring+BITFIELD_FINALIZE) &= ~( 1 << obj->bit_idx);
				UNSET_PAIRED_BITS(obj);
				obuff->use_count--;
				break;
			}
		}
	} while(0);

#ifdef OBJ_DEBUG
	if(obuff->pools && obuff->pools->bitstring) {
		for(bitstring = obuff->pools->bitstring;((void*)bitstring) < ((void*)obuff->pools->head);bitstring+=BITFIELD_SIZE) {
			SYNC_ASSERT(!(*(bitstring) & *(bitstring+BITFIELD_PAIRED)));
		}
	}
#endif

	DO_AUTO_GC_CHECK(obuff);
	OBJ_BUFFER_UNLOCK(obuff);
//	SYNC_ASSERT(ret);
	return ret;
}

#ifdef OBJ_HAS_TOKEN
void*obj_get(struct xultb_obj_factory*obuff, int token) {
	int k, idx;
	int pool_idx;
	SYNC_UWORD8_T*j;
	void*data = NULL;
	struct xultb_obj_pool*pool;

	OBJ_BUFFER_LOCK(obuff);
	do {
		if( (idx = (token - obuff->token_offset)) < 0 ) break;
		k = idx%obuff->pool_size;
		pool_idx = (idx - k)/obuff->pool_size;

		for(pool = obuff->pools;pool;pool = pool->next) {
			if(pool->idx != pool_idx) {
				continue;
			}

			j = pool->head + obuff->obj_size*k;
			if(((struct sync_object*)j)->refcount) {
				CHECK_OBJ((struct sync_object*)j);
				data = (j+sizeof(struct sync_object));
				OBJ_FACTORY_REF(data);
			}
			break;
		}
	} while(0);
	DO_AUTO_GC_CHECK(obuff);
	OBJ_BUFFER_UNLOCK(obuff);
	return data;
}

void xultb_obj_shrink(void*data, int size) {
	struct sync_object*obj = (data - sizeof(struct sync_object));
	int slots;
	struct xultb_obj_factory*obuff = obj->obuff;
	
	size += sizeof(struct sync_object);
	slots = size / obuff->obj_size + ((size % obuff->obj_size)?1:0);
	
	if(!slots || slots > BIT_PER_STRING || slots == obj->slots) {
		return;
	}
	
	CHECK_OBJ(obj);
	OBJ_BUFFER_LOCK(obuff);
	UNSET_PAIRED_BITS(obj);
	
	obj->slots = slots;
	
	SET_PAIRED_BITS(obj);
	if(obuff->shrink) {
		obuff->shrink(data, slots*obuff->obj_size - sizeof(struct sync_object));
	}
	DO_AUTO_GC_CHECK(obuff);
	OBJ_BUFFER_UNLOCK(obuff);
}


void obj_set_flag_by_token(struct xultb_obj_factory*obuff, int token, unsigned int flag) {
	int k, idx;
	int pool_idx;
	SYNC_UWORD8_T*j;
	void*data = NULL;
	struct xultb_obj_pool*pool;

	SYNC_ASSERT(obuff->property & OBJ_FACTORY_EXTENDED);
	OBJ_BUFFER_LOCK(obuff);
	do {
		if( (idx = (token - obuff->token_offset)) < 0 ) break;
		k = idx%obuff->pool_size;
		pool_idx = (idx - k)/obuff->pool_size;

		for(pool = obuff->pools;pool;pool = pool->next) {
			if(pool->idx != pool_idx) {
				continue;
			}

			j = pool->head + obuff->obj_size*k;
			if(((struct sync_object*)j)->refcount) {
				CHECK_OBJ((struct sync_object*)j);
				data = (j+sizeof(struct sync_object));
				xultb_obj_set_flag(data, flag);
			}
			break;
		}
	} while(0);
	DO_AUTO_GC_CHECK(obuff);
	OBJ_BUFFER_UNLOCK(obuff);
}

void obj_unset_flag_by_token(struct xultb_obj_factory*obuff, int token, unsigned int flag) {
	int k, idx;
	int pool_idx;
	SYNC_UWORD8_T*j;
	void*data = NULL;
	struct xultb_obj_pool*pool;

	SYNC_ASSERT(obuff->property & OBJ_FACTORY_EXTENDED);
	OBJ_BUFFER_LOCK(obuff);
	do {
		if( (idx = (token - obuff->token_offset)) < 0 ) break;
		k = idx%obuff->pool_size;
		pool_idx = (idx - k)/obuff->pool_size;

		for(pool = obuff->pools;pool;pool = pool->next) {
			if(pool->idx != pool_idx) {
				continue;
			}

			j = pool->head + obuff->obj_size*k;
			if(((struct sync_object*)j)->refcount) {
				CHECK_OBJ((struct sync_object*)j);
				data = (j+sizeof(struct sync_object));
				obj_unset_flag(data, flag);
			}
			break;
		}
	} while(0);
	DO_AUTO_GC_CHECK(obuff);
	OBJ_BUFFER_UNLOCK(obuff);
}
#endif

static void _obj_unset_flag2(void*data, unsigned int flag, int nocheckpartition) {
	struct sync_object*obj = (data - sizeof(struct sync_object));
	struct xultb_obj_factory*obuff = obj->obuff;
	CHECK_OBJ(obj);
	SYNC_ASSERT(obj->refcount);
	OBJ_BUFFER_LOCK(obuff);
	
	SYNC_ASSERT(obj->obuff->property & OBJ_FACTORY_EXTENDED);
	((struct xultb_object_ext*)(obj+1))->flag &= ~flag;
#ifdef HASH_PARTITION
	if(!nocheckpartition && obj->obuff->property & OBJ_FACTORY_HAS_HASH_PARTITION) {
		SYNC_ASSERT(!(flag & OBJ_ITEM_FLAG_PERFORMANCE1));
		SYNC_ASSERT(!(flag & OBJ_ITEM_FLAG_PERFORMANCE2));
		SYNC_ASSERT(!(flag & OBJ_ITEM_FLAG_PERFORMANCE3));
	}
#endif
#ifndef OBJ_NO_FLAG_OPTIMIZATION
	if(flag & OBJ_ITEM_FLAG_PERFORMANCE) {
		*(obj->bitstring+BITFIELD_PERFORMANCE) &= ~( 1 << obj->bit_idx);
	}
	if(flag & OBJ_ITEM_FLAG_PERFORMANCE1) {
		*(obj->bitstring+BITFIELD_PERFORMANCE1) &= ~( 1 << obj->bit_idx);
	}
	if(flag & OBJ_ITEM_FLAG_PERFORMANCE2) {
		*(obj->bitstring+BITFIELD_PERFORMANCE2) &= ~( 1 << obj->bit_idx);
	}
	if(flag & OBJ_ITEM_FLAG_PERFORMANCE3) {
		*(obj->bitstring+BITFIELD_PERFORMANCE3) &= ~( 1 << obj->bit_idx);
	}
#endif
	DO_AUTO_GC_CHECK(obuff);
	OBJ_BUFFER_UNLOCK(obuff);
}

void obj_unset_flag(void*data, unsigned int flag) {
	_obj_unset_flag2(data, flag, 0);
}

static void _obj_set_flag2(void*data, unsigned int flag, int nocheckpartition) {
	struct sync_object*obj = (data - sizeof(struct sync_object));
	CHECK_OBJ(obj);
	SYNC_ASSERT(obj->refcount);
	OBJ_BUFFER_LOCK(obj->obuff);
	SYNC_ASSERT(obj->obuff->property & OBJ_FACTORY_EXTENDED);
	((struct xultb_object_ext*)(obj+1))->flag |= flag;
#ifdef HASH_PARTITION
	if(!nocheckpartition && (obj->obuff->property & OBJ_FACTORY_HAS_HASH_PARTITION)) {
		SYNC_ASSERT(!(flag & OBJ_ITEM_FLAG_PERFORMANCE1));
		SYNC_ASSERT(!(flag & OBJ_ITEM_FLAG_PERFORMANCE2));
		SYNC_ASSERT(!(flag & OBJ_ITEM_FLAG_PERFORMANCE3));
	}
#endif
#ifndef OBJ_NO_FLAG_OPTIMIZATION
	if(flag & OBJ_ITEM_FLAG_PERFORMANCE) {
		*(obj->bitstring+BITFIELD_PERFORMANCE) |= ( 1 << obj->bit_idx);
	}
	if(flag & OBJ_ITEM_FLAG_PERFORMANCE1) {
		*(obj->bitstring+BITFIELD_PERFORMANCE1) |= ( 1 << obj->bit_idx);
	}
	if(flag & OBJ_ITEM_FLAG_PERFORMANCE2) {
		*(obj->bitstring+BITFIELD_PERFORMANCE2) |= ( 1 << obj->bit_idx);
	}
	if(flag & OBJ_ITEM_FLAG_PERFORMANCE3) {
		*(obj->bitstring+BITFIELD_PERFORMANCE3) |= ( 1 << obj->bit_idx);
	}
#endif
	DO_AUTO_GC_CHECK(obj->obuff);
	OBJ_BUFFER_UNLOCK(obj->obuff);
}


void xultb_obj_set_flag(void*data, unsigned int flag) {
	_obj_set_flag2(data, flag, 0);
}

#define OBJ_GET_PERTITION(hash) ({1<<(hash%3 + 1);})

void xultb_obj_set_hash(void*data, unsigned long hash) {
	struct sync_object*obj = (data - sizeof(struct sync_object));
#ifdef HASH_PARTITION
	unsigned long flag = 0;
#endif
	CHECK_OBJ(obj);
	SYNC_ASSERT(obj->refcount);
	OBJ_BUFFER_LOCK(obj->obuff);
	SYNC_ASSERT(obj->obuff->property & OBJ_FACTORY_EXTENDED);
	((struct xultb_object_ext*)(obj+1))->hash = hash;
#ifdef HASH_PARTITION
	if(obj->obuff->property & OBJ_FACTORY_HAS_HASH_PARTITION) {
		flag = OBJ_GET_PERTITION(hash);
		_obj_unset_flag2(data, OBJ_ITEM_FLAG_PERFORMANCE1 | OBJ_ITEM_FLAG_PERFORMANCE2 | OBJ_ITEM_FLAG_PERFORMANCE3, 1);
		_obj_set_flag2(data, flag, 1);
	}
#endif
	CHECK_OBJ(obj);
	DO_AUTO_GC_CHECK(obj->obuff);
	OBJ_BUFFER_UNLOCK(obj->obuff);
}


#ifdef OBJ_CAN_TEST_FLAG
int obj_test_flag(const void*data, unsigned int flag) {
	const struct sync_object*obj = (data - sizeof(struct sync_object));
	int ret = 0;
	CHECK_OBJ(obj);
	SYNC_ASSERT(obj->refcount);
	OBJ_BUFFER_LOCK(obj->obuff);
	ret = (obj->flag & flag);
	DO_AUTO_GC_CHECK(obj->obuff);
	OBJ_BUFFER_UNLOCK(obj->obuff);
	return ret;
}
#endif

void obj_ref(void*data, const char*filename, int lineno) {
	struct sync_object*obj = (data - sizeof(struct sync_object));
	SYNC_ASSERT(data);
	CHECK_OBJ(obj);
	OBJ_BUFFER_LOCK(obj->obuff);

#ifdef OBJ_DEBUG_REFCOUNT
	obj->rt_idx++;
	obj->rt_idx = obj->rt_idx%OBJ_TRACE_SIZE;
	obj->ref_trace[obj->rt_idx].filename = filename;
	obj->ref_trace[obj->rt_idx].lineno = lineno;
	obj->ref_trace[obj->rt_idx].refcount = obj->refcount;
	obj->ref_trace[obj->rt_idx].op = '+';
#endif

	SYNC_ASSERT(obj->refcount);
	obj->refcount++;
	DO_AUTO_GC_CHECK(obj->obuff);
	OBJ_BUFFER_UNLOCK(obj->obuff);
}

//#ifdef OBJ_BUFFER_HAS_LOCK
void obj_unref_unlocked(void**data, const char*filename, int lineno) {
	struct sync_object*obj = (*data - sizeof(struct sync_object));
#ifdef OBJ_HAS_WEAK
	struct xultb_obj_pool*pool;
#endif
	if(!*data)
		return;
	CHECK_OBJ(obj);
#ifdef OBJ_DEBUG_REFCOUNT
	obj->rt_idx++;
	obj->rt_idx = obj->rt_idx%OBJ_TRACE_SIZE;
	obj->ref_trace[obj->rt_idx].filename = filename;
	obj->ref_trace[obj->rt_idx].lineno = lineno;
	obj->ref_trace[obj->rt_idx].refcount = obj->refcount;
	obj->ref_trace[obj->rt_idx].op = '-';
#endif

	SYNC_ASSERT(obj->refcount);
	obj->refcount--;
	if(!obj->refcount) {
#ifdef OBJ_HAS_WEAK
		for(pool = obj->obuff->pools;pool;pool = pool->next) {
			CHECK_POOL(pool);
			if((void*)obj <= (void*)pool->end && (void*)obj >= (void*)pool->head) {
				pool->weak_obj = obj;
				CHECK_POOL(pool);
				break;
			}
		}
#endif
		*(obj->bitstring) &= ~( 1 << obj->bit_idx);
		if(obj->obuff->property & OBJ_FACTORY_SWEEP_ON_UNREF) {
			if(obj->obuff->finalize)
				obj->obuff->finalize(*data);
			*(obj->bitstring+BITFIELD_FINALIZE) &= ~( 1 << obj->bit_idx);
			UNSET_PAIRED_BITS(obj);
		}
		obj->obuff->use_count--;
	}
	DO_AUTO_GC_CHECK(obj->obuff);
	*data = NULL;
}
//#endif

void obj_unref(void**data, const char*filename, int lineno) {
	struct sync_object*obj = (*data - sizeof(struct sync_object));
	struct xultb_obj_factory*obuff;
#ifdef OBJ_HAS_WEAK
	struct xultb_obj_pool*pool;
#endif
	if(!*data)
		return;
	obuff = obj->obuff;
	OBJ_BUFFER_LOCK(obuff);
	CHECK_OBJ(obj);
#ifdef OBJ_DEBUG_REFCOUNT
	obj->rt_idx++;
	obj->rt_idx = obj->rt_idx%OBJ_TRACE_SIZE;
	obj->ref_trace[obj->rt_idx].filename = filename;
	obj->ref_trace[obj->rt_idx].lineno = lineno;
	obj->ref_trace[obj->rt_idx].refcount = obj->refcount;
	obj->ref_trace[obj->rt_idx].op = '-';
#endif

	SYNC_ASSERT(obj->refcount);
	obj->refcount--;
	if(!obj->refcount) {
#ifdef OBJ_HAS_WEAK
		for(pool = obj->obuff->pools;pool;pool = pool->next) {
			CHECK_POOL(pool);
			if((void*)obj <= (void*)pool->end && (void*)obj >= (void*)pool->head) {
				pool->weak_obj = obj;
				CHECK_POOL(pool);
				break;
			}
		}
#endif
		*(obj->bitstring) &= ~( 1 << obj->bit_idx);
		if(obuff->property & OBJ_FACTORY_SWEEP_ON_UNREF) {
			if(obuff->finalize)
				obuff->finalize(*data);
			*(obj->bitstring+BITFIELD_FINALIZE) &= ~( 1 << obj->bit_idx);
			UNSET_PAIRED_BITS(obj);
		}
		obuff->use_count--;
	}
	*data = NULL;
	DO_AUTO_GC_CHECK(obuff);
	OBJ_BUFFER_UNLOCK(obuff);
}

#ifdef OBJ_HAS_HIJACK
int obj_hijack(void**src, void*dest, const char*filename, int lineno) {
	struct sync_object*obj = (*src - sizeof(struct sync_object));
	int ret = -1;
	struct xultb_obj_factory*obuff = obj->obuff;
#ifdef OBJ_HAS_WEAK
	struct xultb_obj_pool*pool;
#endif
	OBJ_BUFFER_LOCK(obuff);
	do {
		CHECK_OBJ(obj);
#ifdef OBJ_DEBUG_REFCOUNT
		obj->rt_idx++;
		obj->rt_idx = obj->rt_idx%OBJ_TRACE_SIZE;
		obj->ref_trace[obj->rt_idx].filename = filename;
		obj->ref_trace[obj->rt_idx].lineno = lineno;
		obj->ref_trace[obj->rt_idx].refcount = obj->refcount;
		obj->ref_trace[obj->rt_idx].op = 0;
#endif

#ifdef OBJ_DEBUG
		SYNC_ASSERT(obj->refcount);
#endif
		// see if we can do hijack
		if(obj->refcount != 1) {
			break;
		}

		// hijack
		memcpy(dest,*src,obuff->obj_size - sizeof(struct sync_object));
		obj->refcount--;
#ifdef OBJ_HAS_WEAK
		for(pool = obj->obuff->pools;pool;pool = pool->next) {
			CHECK_POOL(pool);
			if((void*)obj <= (void*)pool->end && (void*)obj >= (void*)pool->head) {
				pool->weak_obj = obj;
				CHECK_POOL(pool);
				break;
			}
		}
#endif
		// clear flags
		*(obj->bitstring) &= ~( 1 << obj->bit_idx);
		*(obj->bitstring+BITFIELD_FINALIZE) &= ~( 1 << obj->bit_idx);
		UNSET_PAIRED_BITS(obj);
		obuff->use_count--;
		*src = dest;
		if(obuff->deep_copy)
			obuff->deep_copy(dest);
		ret = 0;
	} while(0);
	DO_AUTO_GC_CHECK(obuff);
	OBJ_BUFFER_UNLOCK(obuff);
	return ret;
}
#endif

void*obj_find_list_full_donot_use(struct xultb_obj_factory*obuff, int (*compare_func)(const void*data, const void*compare_data)
		, const void*compare_data, unsigned int if_flag, unsigned int if_not_flag, unsigned long hash, int shouldref) {
	int k;
	BITSTRING_TYPE*bitstring,bsv;
	SYNC_UWORD16_T oflag;
	SYNC_UWORD16_T bit_idx, obj_idx;
	void *retval = NULL;
	const struct sync_object *obj = NULL;
	const struct obj_list_item*item = NULL;
#ifdef OPTIMIZE_OBJ_LOOP
	int use_count = 0, iteration_count = 0;
#endif
	struct xultb_obj_pool*pool;
	OBJ_BUFFER_LOCK(obuff);
	if(!obuff->use_count) {
		OBJ_BUFFER_UNLOCK(obuff);
		return NULL;
	}
#ifdef OPTIMIZE_OBJ_LOOP
	use_count = obuff->use_count;
#endif
#ifdef HASH_PARTITION
	if(obuff->property & OBJ_FACTORY_HAS_HASH_PARTITION) {
		SYNC_ASSERT(!(if_flag & OBJ_ITEM_FLAG_PERFORMANCE1));
		SYNC_ASSERT(!(if_flag & OBJ_ITEM_FLAG_PERFORMANCE2));
		SYNC_ASSERT(!(if_flag & OBJ_ITEM_FLAG_PERFORMANCE3));
		SYNC_ASSERT(!(if_not_flag & OBJ_ITEM_FLAG_PERFORMANCE1));
		SYNC_ASSERT(!(if_not_flag & OBJ_ITEM_FLAG_PERFORMANCE2));
		SYNC_ASSERT(!(if_not_flag & OBJ_ITEM_FLAG_PERFORMANCE3));
	}
	if((obuff->property & OBJ_FACTORY_HAS_HASH_PARTITION) && hash != 0) {
		if_flag |= OBJ_GET_PERTITION(hash);
	}
#endif
	for(pool = obuff->pools;pool;pool = pool->next) {
#ifdef OPTIMIZE_OBJ_LOOP
		if(iteration_count >= use_count) {
			break;
		}
#endif
		// traverse the bitset
		k = 0;
		bitstring = pool->bitstring;
		for(;BITSTRING_IDX_TO_BITS(k) < obuff->pool_size;k++,bitstring+=BITFIELD_SIZE) {

			bsv = *bitstring;
#ifdef OPTIMIZE_OBJ_LOOP
			if(iteration_count >= use_count) {
				break;
			}
			iteration_count += SYNC_OBJ_POPCOUNT(bsv);
#endif
			// get the bits to finalize
			while(bsv) {
				bit_idx = SYNC_OBJ_CTZ(bsv);
				obj_idx = BITSTRING_IDX_TO_BITS(k) + bit_idx;
				if(obj_idx < obuff->pool_size) {
					obj = (struct sync_object*)(pool->head + obj_idx*obuff->obj_size);
					CHECK_OBJ(obj);
					item = (struct obj_list_item*)(obj+1);
					obj = (item->obj_data - sizeof(struct sync_object));
					CHECK_OBJ(obj);
					if(obuff->property & OBJ_FACTORY_EXTENDED) {
						oflag = ((struct xultb_object_ext*)(obj+1))->flag;
						if(!(oflag & if_flag) || (oflag & if_not_flag) || (hash != 0 && ((struct xultb_object_ext*)(obj+1))->hash != hash)) {
							// clear
							bsv &= ~( 1 << bit_idx);
							continue;
						}
					}
					if(compare_func(item->obj_data, compare_data)) {
						retval = item->obj_data;
						if(shouldref) {
							OBJ_FACTORY_REF(retval);
						}
						goto exit_point;
					}
					CHECK_OBJ(obj);
					// clear
					bsv &= ~( 1 << bit_idx);
				}
			}
		}
	}
	exit_point:
	DO_AUTO_GC_CHECK(obuff);
	OBJ_BUFFER_UNLOCK(obuff);
	return retval;
}


void*obj_find_full(struct xultb_obj_factory*obuff, int (*compare_func)(const void*data, const void*compare_data), const void*compare_data
		, unsigned int if_flag, unsigned int if_not_flag, unsigned long hash, unsigned int shouldref) {
	int k;
	BITSTRING_TYPE*bitstring,bsv;
	void*data;
	SYNC_UWORD16_T oflag;
	SYNC_UWORD16_T bit_idx,obj_idx;
	void *retval = NULL;
	const struct sync_object *obj = NULL;
#ifdef OPTIMIZE_OBJ_LOOP
	int use_count = 0, iteration_count = 0;
#endif
	struct xultb_obj_pool*pool;
	OBJ_BUFFER_LOCK(obuff);
	if(!obuff->use_count) {
		OBJ_BUFFER_UNLOCK(obuff);
		return NULL;
	}
#ifdef OPTIMIZE_OBJ_LOOP
	use_count = obuff->use_count;
#endif

#ifdef HASH_PARTITION
	if(obuff->property & OBJ_FACTORY_HAS_HASH_PARTITION) {
		SYNC_ASSERT(!(if_flag & OBJ_ITEM_FLAG_PERFORMANCE1));
		SYNC_ASSERT(!(if_flag & OBJ_ITEM_FLAG_PERFORMANCE2));
		SYNC_ASSERT(!(if_flag & OBJ_ITEM_FLAG_PERFORMANCE3));
		SYNC_ASSERT(!(if_not_flag & OBJ_ITEM_FLAG_PERFORMANCE1));
		SYNC_ASSERT(!(if_not_flag & OBJ_ITEM_FLAG_PERFORMANCE2));
		SYNC_ASSERT(!(if_not_flag & OBJ_ITEM_FLAG_PERFORMANCE3));
	}
	if((obuff->property & OBJ_FACTORY_HAS_HASH_PARTITION) && hash != 0) {
		if_flag |= OBJ_GET_PERTITION(hash);
	}
#endif
	for(pool = obuff->pools;pool;pool = pool->next) {
#ifdef OPTIMIZE_OBJ_LOOP
		if(iteration_count >= use_count) {
			break;
		}
#endif
		// traverse the bitset
		k = 0;
		bitstring = pool->bitstring;
		for(;BITSTRING_IDX_TO_BITS(k) < obuff->pool_size;k++,bitstring+=BITFIELD_SIZE) {

			bsv = *bitstring;
#ifdef OPTIMIZE_OBJ_LOOP
			if(iteration_count >= use_count) {
				break;
			}
			iteration_count += SYNC_OBJ_POPCOUNT(bsv);
#endif
#ifndef OBJ_NO_FLAG_OPTIMIZATION
			if(obuff->property & OBJ_FACTORY_EXTENDED) {
				if((if_flag & OBJ_ITEM_FLAG_PERFORMANCE) && !(if_flag & ~OBJ_ITEM_FLAG_PERFORMANCE)) {
					bsv &= *(bitstring+BITFIELD_PERFORMANCE);
				}
				if(if_not_flag & OBJ_ITEM_FLAG_PERFORMANCE) {
					bsv &= ~(*(bitstring+BITFIELD_PERFORMANCE));
				}
				if((if_flag & OBJ_ITEM_FLAG_PERFORMANCE1) && !(if_flag & ~OBJ_ITEM_FLAG_PERFORMANCE1)) {
					bsv &= *(bitstring+BITFIELD_PERFORMANCE1);
				}
				if(if_not_flag & OBJ_ITEM_FLAG_PERFORMANCE1) {
					bsv &= ~(*(bitstring+BITFIELD_PERFORMANCE1));
				}
				if((if_flag & OBJ_ITEM_FLAG_PERFORMANCE2) && !(if_flag & ~OBJ_ITEM_FLAG_PERFORMANCE2)) {
					bsv &= *(bitstring+BITFIELD_PERFORMANCE2);
				}
				if(if_not_flag & OBJ_ITEM_FLAG_PERFORMANCE2) {
					bsv &= ~(*(bitstring+BITFIELD_PERFORMANCE2));
				}
				if((if_flag & OBJ_ITEM_FLAG_PERFORMANCE3) && !(if_flag & ~OBJ_ITEM_FLAG_PERFORMANCE3)) {
					bsv &= *(bitstring+BITFIELD_PERFORMANCE3);
				}
				if(if_not_flag & OBJ_ITEM_FLAG_PERFORMANCE3) {
					bsv &= ~(*(bitstring+BITFIELD_PERFORMANCE3));
				}
			}
#endif
			// get the bits to finalize
			while(bsv) {
				bit_idx = SYNC_OBJ_CTZ(bsv);
				obj_idx = BITSTRING_IDX_TO_BITS(k) + bit_idx;
				if(obj_idx < obuff->pool_size) {
					obj = (const struct sync_object*)(pool->head + obj_idx*obuff->obj_size);
					data = (void*)(obj+1);
					CHECK_OBJ(obj);
					if(obuff->property & OBJ_FACTORY_EXTENDED) {
						oflag = ((struct xultb_object_ext*)(obj+1))->flag;
						if(!(oflag & if_flag) || (oflag & if_not_flag) || (hash != 0 && ((struct xultb_object_ext*)(obj+1))->hash != hash)) {
							// clear
							bsv &= ~( 1 << bit_idx);
							continue;
						}
					}
					if((compare_func?compare_func(data, compare_data):1)) {
						retval = data;
						if(shouldref) {
							OBJ_FACTORY_REF(retval);
						}
						goto exit_point;
					}
					CHECK_OBJ(obj);
					// clear
					bsv &= ~( 1 << bit_idx);
				}
			}
		}
	}
	exit_point:
	DO_AUTO_GC_CHECK(obuff);
	OBJ_BUFFER_UNLOCK(obuff);
	return retval;
}

int obj_count_donot_use(const struct xultb_obj_factory*obuff) {
	return obuff->use_count;
}

static void obj_factory_gc_nolock(struct xultb_obj_factory*obuff) {
	int k;
	BITSTRING_TYPE*bitstring,bsv;
	int use_count;
	SYNC_UWORD16_T bit_idx,obj_idx;
	struct sync_object*obj;
	struct xultb_obj_pool*pool,*prev_pool,*next;
	for(pool = obuff->pools, prev_pool = NULL;pool;pool = next) {
		use_count = 0;

		CHECK_POOL(pool);
		next = pool->next;
		// traverse the bitset
		k = 0;
		bitstring = pool->bitstring;
		for(;BITSTRING_IDX_TO_BITS(k) < obuff->pool_size;k++,bitstring+=BITFIELD_SIZE) {

			use_count += SYNC_OBJ_POPCOUNT((*bitstring) & BITSTRING_MASK);
			// get the bits to finalize
			while((bsv = (~(*bitstring)) & (*(bitstring+BITFIELD_FINALIZE)))) {
				bit_idx = SYNC_OBJ_CTZ(bsv);
				obj_idx = BITSTRING_IDX_TO_BITS(k) + bit_idx;
				if(obj_idx < obuff->pool_size) {
					obj = (struct sync_object*)(pool->head + obj_idx*obuff->obj_size);
					if(obuff->finalize) {
						obuff->finalize(obj+1);
					}
					// clear
					*(bitstring+BITFIELD_FINALIZE) &= ~( 1 << bit_idx);
					UNSET_PAIRED_BITS(obj);
				}
			}
		}

		// destroy memory chunk
		if(use_count == 0) {
#ifdef FACTORY_OBJ_SIGN
			obj = (struct sync_object*)pool->head;
			CHECK_OBJ(obj);
#endif
			if(prev_pool) {
				prev_pool->next = pool->next;
			} else {
				obuff->pools = pool->next;
			}
			free(pool);
			pool = NULL;
			obuff->pool_count--;

		} else {
			prev_pool = pool;
		}



	}
}

int obj_exists(struct xultb_obj_factory*obuff, const void*data) {
	struct xultb_obj_pool*pool = NULL;
	int found = 0;
	OBJ_BUFFER_LOCK(obuff);

	for(pool = obuff->pools;pool;pool = pool->next) {
		if(data > (void*)pool->head && data < (void*)pool->end) {
			found = 1;
			break;
		}
	}
	DO_AUTO_GC_CHECK(obuff);
	OBJ_BUFFER_UNLOCK(obuff);
	return found;
}

void xultb_obj_factory_lock_donot_use(struct xultb_obj_factory*obuff) {
	OBJ_BUFFER_LOCK(obuff);
}

void xultb_obj_factory_unlock_donot_use(struct xultb_obj_factory*obuff) {
	OBJ_BUFFER_UNLOCK(obuff);
}

void xultb_obj_factory_gc_donot_use(struct xultb_obj_factory*obuff) {
	OBJ_BUFFER_LOCK(obuff);
	obj_factory_gc_nolock(obuff);
	OBJ_BUFFER_UNLOCK(obuff);
}

void xultb_obj_factory_list_do_full(struct xultb_obj_factory*obuff, int (*obj_do)(void*data, void*func_data), void*func_data
		, unsigned int if_list_flag, unsigned int if_not_list_flag, unsigned int if_flag, unsigned int if_not_flag
		, unsigned long list_hash, unsigned long hash) {
	int k;
	BITSTRING_TYPE*bitstring,bsv;
	SYNC_UWORD16_T oflag;
	SYNC_UWORD16_T bit_idx, obj_idx;
	const struct sync_object *obj = NULL;
	struct obj_list_item*item = NULL;
#ifdef OPTIMIZE_OBJ_LOOP
	int use_count = 0, iteration_count = 0;
#endif
	struct xultb_obj_pool*pool;
	OBJ_BUFFER_LOCK(obuff);
	if(!obuff->use_count) {
		OBJ_BUFFER_UNLOCK(obuff);
		return;
	}
#ifdef OPTIMIZE_OBJ_LOOP
	use_count = obuff->use_count;
#endif
#ifdef HASH_PARTITION
	if(obuff->property & OBJ_FACTORY_HAS_HASH_PARTITION) {
		SYNC_ASSERT(!(if_list_flag & OBJ_ITEM_FLAG_PERFORMANCE1));
		SYNC_ASSERT(!(if_list_flag & OBJ_ITEM_FLAG_PERFORMANCE2));
		SYNC_ASSERT(!(if_list_flag & OBJ_ITEM_FLAG_PERFORMANCE3));
		SYNC_ASSERT(!(if_not_list_flag & OBJ_ITEM_FLAG_PERFORMANCE1));
		SYNC_ASSERT(!(if_not_list_flag & OBJ_ITEM_FLAG_PERFORMANCE2));
		SYNC_ASSERT(!(if_not_list_flag & OBJ_ITEM_FLAG_PERFORMANCE3));
	}
	if((obuff->property & OBJ_FACTORY_HAS_HASH_PARTITION) && hash != 0) {
		if_list_flag |= OBJ_GET_PERTITION(hash);
	}
#endif
	for(pool = obuff->pools;pool;pool = pool->next) {
#ifdef OPTIMIZE_OBJ_LOOP
		if(iteration_count >= use_count) {
			break;
		}
#endif

		// traverse the bitset
		k = 0;
		bitstring = pool->bitstring;
		for(;BITSTRING_IDX_TO_BITS(k) < obuff->pool_size;k++,bitstring+=BITFIELD_SIZE) {

			bsv = *bitstring;
#ifdef OPTIMIZE_OBJ_LOOP
			if(iteration_count >= use_count) {
				break;
			}
			iteration_count += SYNC_OBJ_POPCOUNT(bsv);
#endif
#ifndef OBJ_NO_FLAG_OPTIMIZATION
			if(obuff->property & OBJ_FACTORY_EXTENDED) {
				if((if_list_flag & OBJ_ITEM_FLAG_PERFORMANCE) && !(if_list_flag & ~OBJ_ITEM_FLAG_PERFORMANCE)) {
					bsv &= *(bitstring+BITFIELD_PERFORMANCE);
				}
				if(if_not_list_flag & OBJ_ITEM_FLAG_PERFORMANCE) {
					bsv &= ~(*(bitstring+BITFIELD_PERFORMANCE));
				}
				if((if_list_flag & OBJ_ITEM_FLAG_PERFORMANCE1) && !(if_list_flag & ~OBJ_ITEM_FLAG_PERFORMANCE1)) {
					bsv &= *(bitstring+BITFIELD_PERFORMANCE1);
				}
				if(if_not_list_flag & OBJ_ITEM_FLAG_PERFORMANCE1) {
					bsv &= ~(*(bitstring+BITFIELD_PERFORMANCE1));
				}
				if((if_list_flag & OBJ_ITEM_FLAG_PERFORMANCE2) && !(if_list_flag & ~OBJ_ITEM_FLAG_PERFORMANCE2)) {
					bsv &= *(bitstring+BITFIELD_PERFORMANCE2);
				}
				if(if_not_list_flag & OBJ_ITEM_FLAG_PERFORMANCE2) {
					bsv &= ~(*(bitstring+BITFIELD_PERFORMANCE2));
				}
				if((if_list_flag & OBJ_ITEM_FLAG_PERFORMANCE3) && !(if_list_flag & ~OBJ_ITEM_FLAG_PERFORMANCE3)) {
					bsv &= *(bitstring+BITFIELD_PERFORMANCE3);
				}
				if(if_not_list_flag & OBJ_ITEM_FLAG_PERFORMANCE3) {
					bsv &= ~(*(bitstring+BITFIELD_PERFORMANCE3));
				}
			}
#endif
			// get the bits to finalize
			while(bsv) {
				bit_idx = SYNC_OBJ_CTZ(bsv);
				obj_idx = BITSTRING_IDX_TO_BITS(k) + bit_idx;
				if(obj_idx < obuff->pool_size) {
					obj = (struct sync_object*)(pool->head + obj_idx*obuff->obj_size);
					CHECK_OBJ(obj);
					item = (struct obj_list_item*)(obj+1);
					if(obuff->property & OBJ_FACTORY_EXTENDED) {
						oflag = ((struct xultb_object_ext*)(obj+1))->flag;
						if(!(oflag & if_flag) || (oflag & if_not_flag)) {
							// clear
							bsv &= ~( 1 << bit_idx);
							continue;
						}
					}
					obj = (item->obj_data - sizeof(struct sync_object));
					CHECK_OBJ(obj);
#ifdef HASH_PARTITION
					if(obj->obuff->property & OBJ_FACTORY_HAS_HASH_PARTITION) {
						SYNC_ASSERT(!(if_flag & OBJ_ITEM_FLAG_PERFORMANCE1));
						SYNC_ASSERT(!(if_flag & OBJ_ITEM_FLAG_PERFORMANCE2));
						SYNC_ASSERT(!(if_flag & OBJ_ITEM_FLAG_PERFORMANCE3));
						SYNC_ASSERT(!(if_not_flag & OBJ_ITEM_FLAG_PERFORMANCE1));
						SYNC_ASSERT(!(if_not_flag & OBJ_ITEM_FLAG_PERFORMANCE2));
						SYNC_ASSERT(!(if_not_flag & OBJ_ITEM_FLAG_PERFORMANCE3));
					}
#endif
					if(obj->obuff->property & OBJ_FACTORY_EXTENDED) {
						oflag = ((struct xultb_object_ext*)(obj+1))->flag;
						if(!(oflag & if_flag) || (oflag & if_not_flag) || (hash != 0 && ((struct xultb_object_ext*)(obj+1))->hash != hash)) {
							// clear
							bsv &= ~( 1 << bit_idx);
							continue;
						}
					}
					if(obj_do && obj_do(item, func_data)) {
						goto exitpoint;
					}
					CHECK_OBJ(obj);
					// clear
					bsv &= ~( 1 << bit_idx);
				}
			}
		}
	}
exitpoint:
	DO_AUTO_GC_CHECK(obuff);
	OBJ_BUFFER_UNLOCK(obuff);
}

void xultb_obj_factory_do_full(struct xultb_obj_factory*obuff, int (*obj_do)(void*data, void*func_data), void*func_data, unsigned int if_flag
		, unsigned int if_not_flag, unsigned long hash) {
#if 0
	int i;
	SYNC_UWORD8_T*j;
	OBJ_BUFFER_LOCK(obuff);
	for(i=0;i<OBJ_MAX_POOL_COUNT;i++) {
		if(pool->bitstring) {
			j = pool->head;
			for(;j<pool->end;j+=obuff->obj_size) {
				if(((struct sync_object*)(j))->refcount && (((struct sync_object*)(j))->flag & if_flag) && !(((struct sync_object*)(j))->flag & if_not_flag) && (hash == 0 || hash == ((struct sync_object*)(j))->flag)) {
					if(obj_do && obj_do(j + sizeof(struct sync_object), func_data)) {
						goto exitpoint;
					}
				}
			}
		}
	}
exitpoint:
	OBJ_BUFFER_UNLOCK(obuff);
#else
	int k;
	BITSTRING_TYPE*bitstring,bsv;
	SYNC_UWORD16_T oflag;
	SYNC_UWORD16_T bit_idx, obj_idx;
	const struct sync_object *obj = NULL;
#ifdef OPTIMIZE_OBJ_LOOP
	int use_count = 0, iteration_count = 0;
#endif
	struct xultb_obj_pool*pool;
	OBJ_BUFFER_LOCK(obuff);
	if(!obuff->use_count) {
		OBJ_BUFFER_UNLOCK(obuff);
		return;
	}
#ifdef OPTIMIZE_OBJ_LOOP
	use_count = obuff->use_count;
#endif
#ifdef HASH_PARTITION
	if(obuff->property & OBJ_FACTORY_HAS_HASH_PARTITION) {
		SYNC_ASSERT(!(if_flag & OBJ_ITEM_FLAG_PERFORMANCE1));
		SYNC_ASSERT(!(if_flag & OBJ_ITEM_FLAG_PERFORMANCE2));
		SYNC_ASSERT(!(if_flag & OBJ_ITEM_FLAG_PERFORMANCE3));
		SYNC_ASSERT(!(if_not_flag & OBJ_ITEM_FLAG_PERFORMANCE1));
		SYNC_ASSERT(!(if_not_flag & OBJ_ITEM_FLAG_PERFORMANCE2));
		SYNC_ASSERT(!(if_not_flag & OBJ_ITEM_FLAG_PERFORMANCE3));
	}
	if((obuff->property & OBJ_FACTORY_HAS_HASH_PARTITION) && hash != 0) {
		if_flag |= OBJ_GET_PERTITION(hash);
	}
#endif
	for(pool = obuff->pools;pool;pool = pool->next) {
#ifdef OPTIMIZE_OBJ_LOOP
		if(iteration_count >= use_count) {
			break;
		}
#endif
		// traverse the bitset
		k = 0;
		bitstring = pool->bitstring;
		for(;BITSTRING_IDX_TO_BITS(k) < obuff->pool_size;k++,bitstring+=BITFIELD_SIZE) {

			bsv = *bitstring;
#ifdef OPTIMIZE_OBJ_LOOP
			if(iteration_count >= use_count) {
				break;
			}
			iteration_count += SYNC_OBJ_POPCOUNT(bsv);
#endif
#ifndef OBJ_NO_FLAG_OPTIMIZATION
			if(obuff->property & OBJ_FACTORY_EXTENDED) {
				if((if_flag & OBJ_ITEM_FLAG_PERFORMANCE) && !(if_flag & ~OBJ_ITEM_FLAG_PERFORMANCE)) {
					bsv &= *(bitstring+BITFIELD_PERFORMANCE);
				}
				if(if_not_flag & OBJ_ITEM_FLAG_PERFORMANCE) {
					bsv &= ~(*(bitstring+BITFIELD_PERFORMANCE));
				}
				if((if_flag & OBJ_ITEM_FLAG_PERFORMANCE1) && !(if_flag & ~OBJ_ITEM_FLAG_PERFORMANCE1)) {
					bsv &= *(bitstring+BITFIELD_PERFORMANCE1);
				}
				if(if_not_flag & OBJ_ITEM_FLAG_PERFORMANCE1) {
					bsv &= ~(*(bitstring+BITFIELD_PERFORMANCE1));
				}
				if((if_flag & OBJ_ITEM_FLAG_PERFORMANCE2) && !(if_flag & ~OBJ_ITEM_FLAG_PERFORMANCE2)) {
					bsv &= *(bitstring+BITFIELD_PERFORMANCE2);
				}
				if(if_not_flag & OBJ_ITEM_FLAG_PERFORMANCE2) {
					bsv &= ~(*(bitstring+BITFIELD_PERFORMANCE2));
				}
				if((if_flag & OBJ_ITEM_FLAG_PERFORMANCE3) && !(if_flag & ~OBJ_ITEM_FLAG_PERFORMANCE3)) {
					bsv &= *(bitstring+BITFIELD_PERFORMANCE3);
				}
				if(if_not_flag & OBJ_ITEM_FLAG_PERFORMANCE3) {
					bsv &= ~(*(bitstring+BITFIELD_PERFORMANCE3));
				}
			}
#endif
			// get the bits to finalize
			while(bsv) {
				bit_idx = SYNC_OBJ_CTZ(bsv);
				obj_idx = BITSTRING_IDX_TO_BITS(k) + bit_idx;
				if(obj_idx < obuff->pool_size) {
					obj = (void*)(pool->head + obj_idx*obuff->obj_size);
					CHECK_OBJ(obj);
					if(obuff->property & OBJ_FACTORY_EXTENDED) {
						oflag = ((struct xultb_object_ext*)(obj+1))->flag;
						if(!(oflag & if_flag) || (oflag & if_not_flag) || (hash != 0 && ((struct xultb_object_ext*)(obj+1))->hash != hash)) {
							// clear
							bsv &= ~( 1 << bit_idx);
							continue;
						}
					}
					if(obj_do && obj_do((void*)(obj+1), func_data)) {
						goto exitpoint;
					}
//					CHECK_OBJ(obj);
					// clear
					bsv &= ~( 1 << bit_idx);
				}
			}
		}
	}
exitpoint:
	DO_AUTO_GC_CHECK(obuff);
	OBJ_BUFFER_UNLOCK(obuff);
#endif
}



void xultb_obj_factory_verb(struct xultb_obj_factory*obuff, void (*verb_obj)(const void*data, const void*func_data), const void*func_data, void (*log)(void *log_data, const char*fmt, ...), void*log_data) {
#if 0
	int i, use_count = 0, pool_count = 0;
	SYNC_UWORD8_T*j;
	OBJ_BUFFER_LOCK(obuff);
	for(i=0;i<OBJ_MAX_POOL_COUNT;i++) {
		if(pool->bitstring) {
			j = pool->head;
			for(;j<pool->end;j+=obuff->obj_size) {
				if(((struct sync_object*)(j))->refcount) {
					if(verb_obj)verb_obj(j + sizeof(struct sync_object), func_data);
					use_count++;
				}
			}
			pool_count++;
		}
	}

	if(log)
	log(log_data, "pool count:%d, use count: %d, total memory: %d bytes, total used: %d bytes"
//#ifdef OBJ_DEBUG
	", obuff use count(internal) :%d"
	", obuff slot use count(internal) :%d"
	", bitstring size :%d"
	", pool size :%d"
//#endif
	"\n"
		, pool_count
		, use_count
		, (int)(pool_count*obuff->memory_chunk_size)
		, (int)(obuff->slot_use_count*obuff->obj_size)
//#ifdef OBJ_DEBUG
		, obuff->use_count
		, obuff->slot_use_count
		, (int)obuff->bitstring_size
		, (int)obuff->pool_size
//#endif
	);
	DO_AUTO_GC_CHECK(obuff);
	OBJ_BUFFER_UNLOCK(obuff);
#else
	OBJ_BUFFER_LOCK(obuff);
	xultb_obj_factory_do_full(obuff, (int (*)(void*data, void*func_data))verb_obj, (void*)func_data, OBJ_ITEM_FLAG_ALL, 0, 0);
	if(log)
		log(log_data, "pool count:%d, total memory: %d bytes, used: %d bytes"
		", %d objs"
		", slots :%d"
		", bitstring size :%d"
		", pool size :%d"
		"\n"
			, obuff->pool_count
			, (int)(obuff->pool_count*obuff->memory_chunk_size)
			, (int)(obuff->slot_use_count*obuff->obj_size)
			, obuff->use_count
			, obuff->slot_use_count
			, (int)obuff->bitstring_size
			, (int)obuff->pool_size
		);
	OBJ_BUFFER_UNLOCK(obuff);
#endif
}

void xultb_obj_factory_destroy(struct xultb_obj_factory*obuff) {
	BITSTRING_TYPE*bitstring;
	int k;
	struct xultb_obj_pool*pool;
	if(obuff->sign != OBJ_FACTORY_INITIALIZED) {
		return;
	}

	OBJ_BUFFER_LOCK(obuff);
	for(pool = obuff->pools;pool;pool = pool->next) {
		bitstring = pool->bitstring;
		for(k=0;BITSTRING_IDX_TO_BITS(k) < obuff->pool_size;k++,bitstring+=BITFIELD_SIZE) {
			*bitstring = 0;
		}
	}
	obj_factory_gc_nolock(obuff);
	obuff->sign = 1;
	OBJ_BUFFER_UNLOCK(obuff);
#ifdef OBJ_BUFFER_HAS_LOCK
	if((obuff->property &  OBJ_FACTORY_HAS_LOCK)) {
		xultb_mutex_destroy(&obuff->lock);
	}
#endif
}

void xultb_obj_logger_set(void (*log_func)(enum obj_log_type ltype, char*format, ...)) {
       do_log = log_func;
}

#ifdef TEST_OBJ_FACTORY_UTILS
struct pencil {
	struct xultb_object_ext _ext;
	int color;
	int depth;
};

struct pencil_logger {
	void (*log)(void *log_data, const char*fmt, ...);
	void*log_data;
};

static void pencil_verb(const void*data, const void*func_data) {
	const struct pencil* pen = data;
	const struct pencil_logger*logger = func_data;
	logger->log(logger->log_data, "Pen (color:%d,depth:%d)\n",pen->color,pen->depth);
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

static int pencil_do(void*data, void*func_data) {
	const struct pencil* pen = data;
	const struct pencil_logger*logger = func_data;
	logger->log(logger->log_data, "Pen (color:%d,depth:%d)\n", pen->color,pen->depth);
	return 0;
}

static int obj_utils_test_helper(int inc, struct pencil_logger*logger) {
	struct xultb_obj_factory bpencil;
	struct pencil*pen;
	struct pencil hijacked;
	struct xultb_obj_factory list;
	struct obj_list_item*item = NULL;
	int count = 0;
	int color = 3;
	int idx = 0;
	int depth_list[17] = {1, 2, 2, 3, 4, 5, 0, 7, 8, 9, 3, 4, 5, 6, 7, 8, 9};
	int ret = 0;

	logger->log(logger->log_data, "Testing %d\n", inc);
	xultb_obj_factory_create_full(&bpencil, inc,sizeof(struct pencil), 0, OBJ_FACTORY_EXTENDED, NULL, NULL, NULL, NULL);

	// create 10 pencils
	for(idx = 0; idx < 10; idx++) {
		pen = OBJ_ALLOC1(&bpencil);
		pen->color = (idx%2)?3:1;
		pen->depth = idx;
		SYNC_ASSERT(pen->_ext.flag == OBJ_ITEM_FLAG_ALL);
	}
	// remove 3
	pen = obj_get(&bpencil, 0);
	OBJ_FACTORY_UNREF(pen);
	pen = obj_get(&bpencil, 1);
	OBJ_FACTORY_HIJACK(pen,&hijacked); // hijack one
	pen = obj_get(&bpencil, 6);
	OBJ_FACTORY_UNREF(pen);
	if(inc%2) {
		xultb_obj_factory_gc_donot_use(&bpencil);
	}
	// create more 10
	for(idx = 0; idx < 10; idx++) {
		pen = OBJ_ALLOC1(&bpencil);
		pen->color = (idx%2)?3:1;
		pen->depth = idx;
		SYNC_ASSERT(pen->_ext.flag == OBJ_ITEM_FLAG_ALL);
	}
	xultb_obj_factory_verb(&bpencil, pencil_verb, logger, logger->log, logger->log_data);

	if(!OBJ_FIND(&bpencil, pencil_compare, &color)) {
		if(do_log)do_log(OBJ_LOG_ERROR, "TEST failed while finding color:3\n");
		ret = -1;
	}

	OBJ_LIST_CREATE_NOLOCK_EXT(&list, 10);
	if((count = obj_list_find_from_factory(&bpencil, &list, pencil_compare_all, NULL))) {
		if(count != 17) {
			if(do_log)do_log(OBJ_LOG_ERROR, "TEST failed while finding all\n");
			ret = 1;
		}
		for(idx = 0; (item = obj_get(&list,idx)); idx++) {
			pen = item->obj_data;
			if(pen->depth != depth_list[idx]) {
				if(do_log)do_log(OBJ_LOG_ERROR, "TEST failed while matching depth\n");
				ret = -1;
			}
			//ast_verb(1, "There is a pencil in the list (color:%d,depth:%d)\n", pen->color, pen->depth);
		}
	}
	xultb_obj_factory_destroy(&list);
	OBJ_LIST_CREATE_NOLOCK_EXT(&list, 10);
	if((count = obj_list_find_from_factory(&bpencil, &list, pencil_compare, &color))) {
		if(count != 9) {
			if(do_log)do_log(OBJ_LOG_ERROR, "TEST failed while finding pencil color");
			ret = 1;
		}
		for(idx = 0; (item = obj_get(&list,idx)); idx++) {
			pen = item->obj_data;
			if(pen->color != 3) {
				if(do_log)do_log(OBJ_LOG_ERROR, "TEST failed while checking pencil color\n");
				ret = -1;
			}
			//ast_verb(1, "There is a pencil in the list (color:%d,depth:%d)\n", pen->color, pen->depth);
		}
	}

	xultb_obj_factory_destroy(&list);
	OBJ_FACTORY_DO(&bpencil, pencil_do, logger);

	xultb_obj_factory_gc_donot_use(&bpencil);
	xultb_obj_factory_destroy(&bpencil);
	logger->log(logger->log_data, "Test [OK]\n");

	return ret;
}

static int obj_utils_test_helper2(struct pencil_logger*logger) {
	struct xultb_obj_factory bstrings;
	int i;
	const int CUT_SIZE = 8;
	const char*test_string = "Need help with Red Hat Enterprise Linux? The Global Support Services portal features comprehensive options to getting assistance in managing your Linux system. You've got Red Hat Enterprise Linux, now get the skills. Check out Red Hat's training courses and industry-acclaimed certifications — the most respected certifications in the Linux space. Need help with Red Hat Enterprise Linux? The Global Support Services portal features comprehensive options to getting assistance in managing your Linux system. You've got Red Hat Enterprise Linux, now get the skills. Check out Red Hat's training courses and industry-acclaimed certifications — the most respected certifications in the Linux space.";
	char*string;
	int ret = 0;
	xultb_obj_factory_create_full(&bstrings, 32, 32, 0, OBJ_FACTORY_SWEEP_ON_UNREF, NULL, NULL, NULL, NULL);

	string = xultb_obj_alloc3(&bstrings, strlen(test_string) + 1, NULL);
	strcpy(string, test_string);
	logger->log(logger->log_data, "%s\n", string);

	xultb_obj_shrink(string, CUT_SIZE);
	string[CUT_SIZE-1] = '\0';

	for(i=0;i<6;i++) {
		string = xultb_obj_alloc3(&bstrings, CUT_SIZE, NULL);
		string[CUT_SIZE-1] = '\0';
		logger->log(logger->log_data, "%s\n", string);

		if(strstr(test_string, string)) {
			logger->log(logger->log_data, "Test [OK]\n");
		} else {
			logger->log(logger->log_data, "Test 2 [failed]\n");
			ret = -1;
		}
	}

	xultb_obj_factory_destroy(&bstrings);
	return ret;
}

static int obj_utils_test_helper4(struct pencil_logger*logger) {
	struct xultb_obj_factory bstrings;
	struct xultb_obj_queue queue;
	const int CUT_SIZE = 8;
	const char*test_string = "Need help with Red Hat Enterprise Linux? The Global Support Services portal features comprehensive options to getting assistance in managing your Linux system. You've got Red Hat Enterprise Linux, now get the skills. Check out Red Hat's training courses and industry-acclaimed certifications — the most respected certifications in the Linux space. Need help with Red Hat Enterprise Linux? The Global Support Services portal features comprehensive options to getting assistance in managing your Linux system. You've got Red Hat Enterprise Linux, now get the skills. Check out Red Hat's training courses and industry-acclaimed certifications — the most respected certifications in the Linux space.";
	char*string;
	int ret = 0, i;
	xultb_obj_queue_init(&queue);
	xultb_obj_factory_create_full(&bstrings, 32, 2, 0, OBJ_FACTORY_SWEEP_ON_UNREF, NULL, NULL, NULL, NULL);

	string = xultb_obj_alloc3(&bstrings, strlen(test_string) + 1, NULL);
	strcpy(string, test_string);
	logger->log(logger->log_data, "%s\n", string);

	xultb_obj_shrink(string, CUT_SIZE);
	string[CUT_SIZE-1] = '\0';

	for(i=0;i<6;i++) {
		string = xultb_obj_alloc3(&bstrings, CUT_SIZE, NULL);
		string[CUT_SIZE-1] = '\0';
		xultb_obj_enqueue(&queue, string);
	}
	
	while((string = xultb_obj_dequeue(&queue))) {
		logger->log(logger->log_data, "%s\n", string);
		if(strstr(test_string, string)) {
			logger->log(logger->log_data, "~ [OK]\n");
		} else {
			logger->log(logger->log_data, "Test 4 [failed]\n");
			ret = -1;
		}
		OBJ_FACTORY_UNREF(string);
	}
	
	xultb_obj_queue_destroy(&queue);
	xultb_obj_factory_destroy(&bstrings);
	return ret;
}

static int obj_utils_test_helper3(struct pencil_logger*logger) {
	struct xultb_obj_factory bstrings;
	const int CUT_SIZE = 8;
	const char*test_string = "Need help with Red Hat Enterprise Linux? The Global Support Services portal features comprehensive options to getting assistance in managing your Linux system. You've got Red Hat Enterprise Linux, now get the skills. Check out Red Hat's training courses and industry-acclaimed certifications — the most respected certifications in the Linux space. Need help with Red Hat Enterprise Linux? The Global Support Services portal features comprehensive options to getting assistance in managing your Linux system. You've got Red Hat Enterprise Linux, now get the skills. Check out Red Hat's training courses and industry-acclaimed certifications — the most respected certifications in the Linux space.";
	char*string, *string2, *string3;
	int ret = 0;
	xultb_obj_factory_create_full(&bstrings, 256, 2, 0, OBJ_FACTORY_SWEEP_ON_UNREF, NULL, NULL, NULL, NULL);

	string = xultb_obj_alloc3(&bstrings, strlen(test_string) + 1, NULL);
	strcpy(string, test_string);
	logger->log(logger->log_data, "%s\n", string);

	string2 = xultb_obj_alloc3(&bstrings, CUT_SIZE, NULL);
	logger->log(logger->log_data, "%s\n", string);

	if(strcmp(string, test_string)) {
		logger->log(logger->log_data, "Test 3 [failed]\n");
		ret = -1;
	}

	xultb_obj_shrink(string, CUT_SIZE<<1);
	string3 = xultb_obj_alloc3(&bstrings, CUT_SIZE, NULL);
	OBJ_FACTORY_UNREF(string);
	string = xultb_obj_alloc3(&bstrings, CUT_SIZE, NULL);
	
	xultb_obj_factory_destroy(&bstrings);
	return ret;
}

int obj_utils_test(void (*alog)(void *log_data, const char*fmt, ...), void*alog_data) {
	int i;
	struct pencil_logger logger = {
		.log = alog,
		.log_data = alog_data,
	};

	xultb_obj_queuesystem_init();
	for(i=30;i>6;i--) {
		if(obj_utils_test_helper(i, &logger) != 0) {
			return -1;
		}
	}
	if(obj_utils_test_helper2(&logger)) {
		return -1;
	}
	if(obj_utils_test_helper3(&logger)) {
		return -1;
	}
	if(obj_utils_test_helper4(&logger)) {
		return -1;
	}
	alog(alog_data, "Test Fully [OK]\n");
	return 0;
}

#endif

#ifdef __cplusplus
}
#endif
