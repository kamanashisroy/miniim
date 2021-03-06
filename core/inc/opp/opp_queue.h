/*
 * obj_queue.h
 *
 *  Created on: Feb 9, 2011
 *      Author: Kamanashis Roy
 */

#ifndef OPP_QUEUE_H
#define OPP_QUEUE_H
#include "core/config.h"
#include "core/thread.h"

C_CAPSULE_START

enum {
	OBJ_QUEUE_RETRUN_UNLINK = -99,
	OBJ_QUEUE_STACK_ALLOC = 255,
};

#ifdef SYNC_HAS_ATOMIC_OPERATION
#define SYNC_USE_LOCKFREE_QUEUE
#endif

#ifdef SYNC_USE_LOCKFREE_QUEUE
#define SYNC_QUEUE_VOLATILE_VAR volatile
#else
#define SYNC_QUEUE_VOLATILE_VAR
#endif

struct opp_queue_item {
	void*obj_data;
	SYNC_QUEUE_VOLATILE_VAR struct opp_queue_item*_next;
};

/* using freelist solves the repeatative memory allocation and locking problem .. anyway it creates fragmentation */
//#define USE_FREELIST

struct opp_queue {
	SYNC_QUEUE_VOLATILE_VAR struct opp_queue_item*_tail;
#ifdef SYNC_USE_LOCKFREE_QUEUE
#ifdef USE_FREELIST
	SYNC_QUEUE_VOLATILE_VAR struct opp_queue_item*_freelist;
#endif
	struct opp_queue_item _head_node,_free_node;
#else
	struct opp_queue_item*_first;
#endif
	SYNC_UWORD16_T _use_count;
	SYNC_UWORD8_T _sign;
	SYNC_UWORD8_T _factory_idx;
};

#define OPP_QUEUE_SIZE(q) ({(q)->_use_count;})

int opp_enqueue(struct opp_queue*queue, void*obj_data);
void*opp_dequeue(struct opp_queue*queue);
#if 0
int opp_queue_init(struct opp_queue*queue);
#endif
int opp_queue_init2(struct opp_queue*queue, int scindex);
int opp_queue_deinit(struct opp_queue*queue);
int opp_queue_do_full_on_stack(struct opp_queue*queue, int (*func)(void*data, void*func_data), void*func_data);
int opp_queue_do_full(struct opp_queue*queue, int (*func)(void*data, void*func_data), void*func_data);
int opp_queue_do_full_unsafe(struct opp_queue*queue, int (*func)(void*data, void*func_data), void*func_data);

void opp_queuesystem_verb(void (*log)(void *log_data, const char*fmt, ...), void*log_data);
int opp_queuesystem_init();
void opp_queuesystem_deinit();

#define OPP_QUEUE_DECLARE_STACK(x,y) char _x##buf[sizeof(struct opp_queue)+sizeof(struct opp_factory)];struct opp_queue*x = (struct opp_queue*)_x##buf;struct opp_factory*_x##fac = (struct opp_factory*)(x+1);do { \
	_x##fac->sign = 0; \
	SYNC_ASSERT(!opp_factory_create_full(_x##fac, y, sizeof(struct opp_queue_item), 1, OPPF_SWEEP_ON_UNREF, NULL)); \
	opp_factory_create_pool_donot_use(_x##fac, NULL, alloca(_x##fac->memory_chunk_size)); \
	opp_queue_init2(x, OBJ_QUEUE_STACK_ALLOC); \
}while(0);

#define OPP_QUEUE_DESTROY_STACK(x) ({opp_queue_deinit(x);opp_factory_destroy(_x##fac);})

#ifdef TEST_OBJ_FACTORY_UTILS
void*opp_queue_test_thread_run(void*notused);
int opp_queue_test_init();
int opp_queue_test_deinit();
/*
  	const int threadcount = 10;
	int i;
	pthread_t ths[100];
	pthread_attr_t att[100];
	opp_queue_test_init();
	SYNC_ASSERT(threadcount <= 100);
	printf("Starting threads\n");
	for(i = 0; i < threadcount; i++) {
		pthread_attr_init(att+i);
		pthread_attr_setdetachstate(att+i, PTHREAD_CREATE_JOINABLE);
		pthread_create(ths+i, att+i, opp_queue_test_thread_run, NULL);
	}
	opp_queue_test_thread_run(NULL);
	for(i = 0; i < threadcount; i++) {
		SYNC_ASSERT(!pthread_join(ths[i], NULL));
		pthread_attr_destroy(att+i);
	}
	printf("If you see this then the error is fixed\n");
	opp_queue_test_deinit();

 * */
#endif


C_CAPSULE_END

#endif /* OPP_QUEUE_H */
