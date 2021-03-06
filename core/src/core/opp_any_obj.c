/*
 * obj_watchdog.h
 *
 *  Created on: Jun 17, 2011
 *      Author: ayaskanti
 */

#ifndef OPP_ANY_OBJ_H_
#define OPP_ANY_OBJ_H_


#include "core/config.h"
#include "opp/opp_any_obj.h"
#include "opp/opp_factory.h"
#include "opp/opp_type.h"

C_CAPSULE_START

//opp_vtable_declare(any_obj,);
//opp_class_declare(any_obj,);
struct any_obj {
	opp_callback_t cb;
};

static struct opp_factory objs;
void*opp_any_obj_alloc(int size, opp_callback_t cb, void*arg, ...) {
	void*obj = opp_alloc4(&objs, size+sizeof(struct any_obj), 0, cb);
	va_list ap;
	va_start(ap, arg);
	cb(obj, OPPN_ACTION_INITIALIZE, arg, ap, size);
	va_end(ap);
	return obj;
}

OPP_CB(any_obj) {
	void*obj = data;
	switch(callback) {
	case OPPN_ACTION_INITIALIZE:
	{
		struct any_obj*cb = (data+size);
		cb--;
		cb->cb = cb_data;
//		obj->vtable->oppcb = cb_data;
		break;
	}
	case OPPN_ACTION_FINALIZE:
	{
		struct any_obj*cb = (data+size);
		cb--;
		cb->cb(obj, OPPN_ACTION_FINALIZE, NULL, ap, size - sizeof(struct any_obj));
		break;
	}
	}
	return 0;
}

void opp_any_obj_system_init() {
	SYNC_ASSERT(!OPP_FACTORY_CREATE(
		&objs
		, 1,32
		, OPP_CB_FUNC(any_obj))
	);
}

void opp_any_obj_system_deinit() {
	opp_factory_destroy(&objs);
}


C_CAPSULE_END

#endif /* OPP_ANY_OBJ_H_ */
