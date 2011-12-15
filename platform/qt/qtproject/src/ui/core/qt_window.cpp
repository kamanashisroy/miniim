
#include "config.h"
#include "core/xultb_obj_factory.h"
#include "core/logger.h"
#include "ui/xultb_guicore.h"
#include "ui/core/xultb_list.h"
#include <QtGui>
#include "qt_window.h"
#include "qt_graphics.h"

C_CAPSULE_START

#define TO_QT_WIN(x) (QtXulTbWindow*)(x+1)

static struct opp_factory window_factory;
static void qt_window_impl_paint_wrapper(struct xultb_window*win, struct xultb_graphics*g) {
	QtXulTbWindow*qw = TO_QT_WIN(win);
    // TODO see if the xultb_graphics is initiated ..
    SYNC_LOG(SYNC_VERB, "Painting list\n");
    qw->proto_paint(win, g);
    QTG_CAPSULE(
    		qw->setPage(qtg->page);
    		qtg->painter->end();
    );
    SYNC_LOG(SYNC_VERB, "Rendering list(TODO: refresh now)\n");
    //opp_callback(g, OPPN_ACTION_GUI_RENDER, ql->canvas);
}

static void qt_window_impl_show(struct xultb_window*win) {
	QtXulTbWindow*qw = TO_QT_WIN(win);
    xultb_guicore_set_dirty(win);
    xultb_guicore_walk(0); // XXX should I force it to render ??
    SYNC_LOG(SYNC_VERB, "Showing Window ..\n");
    qw->show();
}


struct xultb_window*xultb_window_platform_create(int (*callback)(void*data, int callback, void*cb_data, va_list ap)) {
	struct xultb_window*win = (struct xultb_window*)opp_alloc4(&window_factory, 0, 0, callback);
    return 0;
}


OPP_CB(qt_window) {
	struct xultb_window*win = (struct xultb_window*)data;
	QtXulTbWindow*qw = TO_QT_WIN(win);
	switch(callback) {
	case OPPN_ACTION_INITIALIZE:
	{
		int (*proto)(void*data, int callback, void*cb_data, va_list ap) = init_data;
		va_args ap;
		proto(data, OPPN_ACTION_INITIALIZE, NULL, ap);
		qw->proto_paint = win->vtable->paint;
		win->vtable->paint = qt_window_impl_paint_wrapper;
		win->vtable->show = qt_window_impl_show;
	}
		return 0;
	case OPPN_ACTION_FINALIZE:
		break;
	}
	return 0;
}


int xultb_window_system_platform_init() {
	SYNC_ASSERT(
			OPP_FACTORY_CREATE(&window_factory
			,1
			,sizeof(struct xultb_window)+sizeof(QtXulTbWindow)
			, OPP_CB_FUNC(qt_window)
		) == 0);
	return 0;
}

C_CAPSULE_END

