/*
 * xultb_gui_input.c
 *
 *  Created on: Dec 26, 2011
 *      Author: ayaskanti
 */

#include "opp/opp_indexed_list.h"
#include "opp/opp_salt.h"
#include "ui/xultb_gui_input.h"
#include "ui/xultb_guicore.h"
#include "rtree/index.h"
#include "core/logger.h"

struct xultb_gui_input_event {
	struct xultb_window*target;
	void*var;
	int flags;
	int key_code;
	int x,y;
};

static struct opp_factory targets;
static struct Node*root = NULL;
static struct xultb_window*target_win = NULL;
static int handle_event_impl_helper(int id, void* arg) {
	struct xultb_gui_input_event*evt = arg;
	// Note: -1 to make up for the +1 when data was inserted
	//printf("Hit data rect %d\n", id-1);
	void*var;
	opp_at_ncode2(var, void*, &targets, id-1,
		target_win->vtable->handle_event(target_win, var, evt->flags, evt->key_code, evt->x, evt->y);
	);
//	return 1; // keep going
	return 0;
}

#define SET_BOUNDARY(var,x,y,x1,y1) ({var.boundary[0] = x;var.boundary[1] = y;var.boundary[2] = x1;var.boundary[3] = y1;})

static int handle_event_impl(int flags, int key_code, int x, int y) {
//	GUI_INPUT_LOG( "See what we can do ..\n");
	if(!target_win) {
		return 0;
	}
	if(flags & XULTB_INPUT_SCREEN_EVENT) {
		struct xultb_gui_input_event evt = {
				.flags = flags,
				.key_code = key_code,
				.x = x,
				.y = y,
		};
		struct Rect search_rect;// XXX will it work from stack ? or do we need memory allocation ?
		SET_BOUNDARY(search_rect,x,y,x+1,y+1);
//		nhits =
		GUI_INPUT_LOG("Finding point:(%d,%d)\n", x, y);
		RTreeSearch(root, &search_rect, handle_event_impl_helper, &evt);
	} else {
		target_win->vtable->handle_event(target_win, NULL, flags, key_code, x, y);
	}
	return 0;
}

int xultb_gui_input_register_action(void*data, int x, int y, int x2, int y2) {
	int i = OPP_FACTORY_USE_COUNT(&targets);
	struct Rect new_rect; // XXX will it work from stack ? or do we need memory allocation ?
	SYNC_ASSERT(target_win);
	SET_BOUNDARY(new_rect,x,y,x2,y2);
	RTreeInsertRect(&new_rect, i+1, &root, 0);
	opp_indexed_list_set(&targets, i, data);
	GUI_INPUT_LOG("Adding rect:(%d,%d,%d,%d)\n", x, y, x2, y2);
	return 0;
}

int xultb_gui_input_reset(struct xultb_window*win) {
	int i, count = OPP_FACTORY_USE_COUNT(&targets);
	SYNC_ASSERT(win);
	for(i = 0; i < count; i++) {
		opp_indexed_list_set(&targets, i, NULL);
	}
	if(root) {
		RTreeFreeNode(root); // God it does not free the children ! TODO use obj in this RTree and destroy the factory
	}
	root = RTreeNewIndex();
	if(target_win) {
		OPPUNREF(target_win);
	}
	target_win = OPPREF(win);
	return 0;
}

int xultb_gui_input_init() {
	opp_indexed_list_create2(&targets, 4);
	root = RTreeNewIndex();
	xultb_gui_input_platform_init(handle_event_impl);
	return 0;
}
