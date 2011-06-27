/*
 * obj_factory_list.h
 *
 *  Created on: Feb 9, 2011
 *      Author: Kamanashis Roy
 */

#ifndef OBJ_FACTORY_QUEUE_H_
#define OBJ_FACTORY_QUEUE_H_

enum {
	OBJ_QUEUE_RETRUN_UNLINK = -99,
};

struct xultb_obj_queue {
	struct obj_queue_item*_first;
	struct obj_queue_item*_last;
	unsigned int _use_count;
	char _sign;
};

#define OBJ_QUEUE_SIZE(q) ({(q)->_use_count;})

int xultb_obj_enqueue(struct xultb_obj_queue*queue, void*obj_data);
void*xultb_obj_dequeue(struct xultb_obj_queue*queue);
int xultb_obj_queue_init(struct xultb_obj_queue*queue);
int xultb_obj_queue_destroy(struct xultb_obj_queue*queue);
int xultb_obj_queue_do_full(struct xultb_obj_queue*queue, int (*func)(void*data, void*func_data), void*func_data);
int xultb_obj_queuesystem_init();
void xultb_obj_queuesystem_deinit();
#endif /* OBJ_FACTORY_QUEUE_H_ */
