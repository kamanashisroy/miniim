/*
 * xultb_resource.h
 *
 *  Created on: Dec 28, 2011
 *      Author: ayaskanti
 */

#ifndef XULTB_RESOURCE_H_
#define XULTB_RESOURCE_H_

#include "core/config.h"
#include "core/xultb_exttypes.h"
#include "core/xultb_decorator.h"
#include "opp/opp_queue.h"
#include "opp/opp_type.h"
#include "ui/xultb_guicore.h"

C_CAPSULE_START

enum xultb_resource_type {
	XULTB_RESOURCE_IMG = 1,
	XULTB_RESOURCE_DOM = 2,
	XULTB_RESOURCE_UNKNOWN = 3,
};

opp_vtable_declare(xultb_resource_id,
	void*cb_data;
	void (*handle_content)(void*cb_data, xultb_str_t*url, enum xultb_resource_type type, void*obj);
	void (*handle_error)(void*cb_data, xultb_str_t*url, enum xultb_resource_type type, int code, xultb_str_t*reason);
);

opp_class_declare(xultb_resource_id,
	enum xultb_resource_type type;
	xultb_str_t*url;
);

opp_vtable_declare(xultb_resource_engine,
);

opp_class_declare(xultb_resource_engine,
	opp_class_extend(struct xultb_gui_task);
	struct opp_queue ids;
	xultb_str_t*prefix;
);

#define RESOURCE_LOG(...) SYNC_LOG(SYNC_VERB, __VA_ARGS__)
//#define RESOURCE_LOG(...)

struct xultb_resource_id*xultb_resource_id_create(xultb_str_t*base, xultb_str_t*url
		, struct opp_vtable_xultb_resource_id*vtable);
int xultb_resource_engine_push(struct xultb_resource_id*id);
int xultb_resource_engine_register(struct xultb_resource_engine*module);
#if 0
struct xultb_resource_engine*xultb_resource_engine_create();
#endif
int xultb_resource_engine_init();

C_CAPSULE_END

#endif /* XULTB_RESOURCE_H_ */
