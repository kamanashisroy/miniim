/*
 * xultb_resource.c
 *
 *  Created on: Dec 29, 2011
 *      Author: ayaskanti
 */

#include "io/xultb_resource.h"
#include "opp/opp_factory.h"
#include "opp/opp_queue.h"
#include "core/logger.h"
#include "ui/page/xultb_page.h"
#include "opp/opp_list.h"
#include "opp/opp_iterator.h"
#include "opp/opp_any_obj.h"

C_CAPSULE_START

static struct opp_factory resource_id_factory;
struct xultb_resource_id*xultb_resource_id_create(xultb_str_t*base, xultb_str_t*url
		, struct opp_vtable_xultb_resource_id*vtable) {
	RESOURCE_LOG("Creating id:%s\n", url->str);
	return opp_alloc4(&resource_id_factory, 0, 0, base, url, vtable);
}

int xultb_resource_engine_push_helper(const void*data, const void*func_data) {
	const struct xultb_resource_id*id = func_data;
	const struct xultb_resource_engine*engine = ((struct opp_list_item*)data)->obj_data;
	RESOURCE_LOG("Using engine:%s\n", engine->prefix->str);
	if(engine->prefix->len <= id->url->len && !memcmp(engine->prefix->str, id->url->str, engine->prefix->len)) {
		return 1;
	}
	return 0;
}

static struct opp_factory engines;
struct xultb_resource_engine*default_engine = NULL;
int xultb_resource_engine_push(struct xultb_resource_id*id) {
	RESOURCE_LOG("Requesting resource:%s\n", id->url->str);
	struct xultb_resource_engine*engine = opp_find_full(&engines
			, xultb_resource_engine_push_helper, id, OPPN_ALL, 0, 0, 0);
	if(engine) {
		RESOURCE_LOG("Engine:%s\n", engine->prefix->str);
		opp_enqueue(&engine->ids, id);
	} else if(default_engine){
		RESOURCE_LOG("Using default engine\n");
		opp_enqueue(&default_engine->ids, id);
	} else {
		SYNC_LOG(SYNC_ERROR, "No appropriate engine found\n");
	}
	return 0;
}

int xultb_resource_engine_register(struct xultb_resource_engine*module) {
	return opp_alloc4(&engines, 0, 0, module)?-1:0;
//	TODO opp_set_hash(item, );
}

static int xultb_resource_run(void*cb_data, int ms) {
	struct xultb_resource_engine*engine = cb_data;
	struct xultb_resource_id*id;
	while((id = opp_dequeue(&engine->ids))) {
		struct xultb_ml_node*root = xultb_ml_parse(id->url->str);
		if(root) {
			id->vtable->handle_content(id->vtable->cb_data, id->url, XULTB_RESOURCE_DOM, root);
		} else {
			id->vtable->handle_error(id->vtable->cb_data, id->url, XULTB_RESOURCE_DOM, 0, NULL);
		}
		OPPUNREF(id);
	}
	return 0;
}


OPP_CB(xultb_resource_engine);
opp_vtable_define(xultb_resource_engine,
	.oppcb = OPP_CB_FUNC(xultb_resource_engine)
);

OPP_CB(xultb_resource_engine) {
	struct xultb_resource_engine*engine = (struct xultb_resource_engine*)data;
	// do a cleanup
	memset(engine, 0, sizeof(struct xultb_resource_engine));
	switch(callback) {
	case OPPN_ACTION_INITIALIZE:
	{
		engine->super_data.cb_run = xultb_resource_run;
		opp_queue_init2(&engine->ids, 0);
		xultb_guicore_register_task(&engine->super_data);
		opp_vtable_set(engine, xultb_resource_engine);
		return 0;
	}
	case OPPN_ACTION_FINALIZE:
		xultb_guicore_unregister_task(&engine->super_data);
		opp_queue_deinit(&engine->ids);
		break;
	}
	return 0;
}


OPP_CB(xultb_resource_id) {
	struct xultb_resource_id*id = (struct xultb_resource_id*)data;
	// do a cleanup
	memset(id, 0, sizeof(struct xultb_resource_id));
	switch(callback) {
	case OPPN_ACTION_INITIALIZE:
	{
		xultb_str_t*base = cb_data;
		xultb_str_t*url = va_arg(ap, xultb_str_t*);
		if(!url) {
			return -1;
		}
//		RESOURCE_LOG("Creating id.....:%s\n", url->str);
		id->url = xultb_str_alloc(NULL, (base?base->len:0) + url->len, NULL, 0);
		if(base) {
			xultb_str_cat(id->url, base);
		}
		xultb_str_cat(id->url, url);
		id->vtable = va_arg(ap, void*);
		// TODO identify the resource type ..
		id->type = XULTB_RESOURCE_UNKNOWN;
		RESOURCE_LOG("requested url:%s\n", id->url->str);
		return 0;
	}
	case OPPN_ACTION_FINALIZE:
		OPPUNREF(id->url);
		break;
	}
	return 0;
}


int xultb_resource_engine_init() {
	SYNC_ASSERT(!OPP_FACTORY_CREATE(
		&resource_id_factory
		, 1,sizeof(struct xultb_resource_id)
		, OPP_CB_FUNC(xultb_resource_id))
	);
	SYNC_ASSERT(!opp_list_create2(&engines, 4, 0));
	default_engine = (struct xultb_resource_engine*)opp_any_obj_alloc(sizeof(*default_engine)
			, vtable_xultb_resource_engine.oppcb, NULL);
	return 0;
}

C_CAPSULE_END
