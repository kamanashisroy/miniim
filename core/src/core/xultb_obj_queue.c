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
#include "sync/obj_queue.h"

enum {
	OBJ_QUEUE_INITIALIZED = 14,
#ifdef SYNC_LOW_MEMORY
	OBJ_QUEUE_BUFFER_INC = 32
#else
	OBJ_QUEUE_BUFFER_INC = 1024
#endif
};

struct obj_queue_item {
	void*obj_data;
	void*_next;
};

static struct obj_factory queue_factory;

int xultb_obj_enqueue(struct xultb_obj_queue*queue, void*obj_data) {
	struct obj_queue_item*item = NULL;
	/* sanity check */
	if(!obj_data) {
		return 0;
	}
	xultb_obj_factory_lock_donot_use(&queue_factory);
	do {
		item = OBJ_ALLOC2(&queue_factory, obj_data);
		if(!item) {
			break;
		}
		if (queue->_first == NULL) {
			queue->_first = queue->_last = item;
		} else {
			queue->_last->_next = item;
			queue->_last = item;
		}
		queue->_use_count++;
	} while(0);
	xultb_obj_factory_unlock_donot_use(&queue_factory);
	return item?0:-1;
}

void*xultb_obj_dequeue(struct xultb_obj_queue*queue) {
	void*ret = NULL;
	struct obj_queue_item *item = NULL;
	
	if(!queue->_first) {
		return NULL;
	}

	xultb_obj_factory_lock_donot_use(&queue_factory);
	item = queue->_first;
	do {
		if(!item) {
			break;
		}
		ret = item->obj_data;
		OBJ_FACTORY_REF(ret);
		queue->_first = item->_next;
		OBJ_FACTORY_UNREF_UNLOCKED(item);
		queue->_use_count--;
	} while(0);
	xultb_obj_factory_unlock_donot_use(&queue_factory);
	return ret;
}

int xultb_obj_queue_do_full(struct xultb_obj_queue*queue, int (*func)(void*data, void*func_data), void*func_data) {
	struct obj_queue_item*item, *prev;
	if(queue->_sign != OBJ_QUEUE_INITIALIZED) {
		return 0;
	}
	xultb_obj_factory_lock_donot_use(&queue_factory);
	for(item = queue->_first, prev = NULL;item;) {
		if(func(item->obj_data, func_data) == OBJ_QUEUE_RETRUN_UNLINK) {
			if(prev) {
				prev->_next = item->_next;
			} else {
				queue->_first = item->_next;
			}
			OBJ_FACTORY_UNREF_UNLOCKED(item);
			queue->_use_count--;
			item = prev?prev->_next:queue->_first;
		} else {
			prev = item;
			item = item->_next;
		}
	}
	xultb_obj_factory_unlock_donot_use(&queue_factory);
	return 0;
}

static int obj_queue_item_finalize(void*data) {
	struct obj_queue_item*item = data;
	OBJ_FACTORY_UNREF(item->obj_data);
	item->_next = NULL;
	return 0;
}

static int obj_queue_item_initialize(void*data, const void*obj_data, unsigned short notused) {
	struct obj_queue_item*item = data;
	SYNC_ASSERT(obj_data);
	item->obj_data = (void*)obj_data;
	OBJ_FACTORY_REF(item->obj_data);
	item->_next = NULL;
	return 0;
}

int xultb_obj_queue_init(struct xultb_obj_queue*queue) {
	queue->_use_count = 0;
	queue->_first = queue->_last = NULL;
	queue->_sign = OBJ_QUEUE_INITIALIZED;
	return 0;
}

int xultb_obj_queue_destroy(struct xultb_obj_queue*queue) {
	struct obj_queue_item*item,*next;
	int obj_count, reduced;
	if(queue->_sign == OBJ_QUEUE_INITIALIZED) {
		obj_count = queue_factory.use_count;
		reduced = 0;
		xultb_obj_factory_lock_donot_use(&queue_factory);
		for(item = queue->_first;item;item = next) {
			next = item->_next;
			OBJ_FACTORY_UNREF_UNLOCKED(item);
			reduced++;
		}
		SYNC_ASSERT((obj_count - reduced) == queue_factory.use_count);
		xultb_obj_factory_unlock_donot_use(&queue_factory);
		queue->_first = queue->_last = NULL;
		queue->_sign = 89;
	}
	return 0;
}

int xultb_obj_queuesystem_init() {
	return obj_factory_create_full(&queue_factory, OBJ_QUEUE_BUFFER_INC
			, sizeof(struct obj_queue_item)
			, 1/*token offset*/, OBJ_FACTORY_HAS_LOCK | OBJ_FACTORY_SWEEP_ON_UNREF
			, obj_queue_item_initialize
			, obj_queue_item_finalize, NULL, NULL);
}

void xultb_obj_queuesystem_deinit() {
	xultb_obj_factory_destroy(&queue_factory);
}

#ifdef __cplusplus
}
#endif
