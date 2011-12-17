
#include "config.h"
#include "core/xultb_obj_factory.h"
#include "core/logger.h"
#include "ui/xultb_guicore.h"
#include "ui/core/xultb_list.h"
#include <QtGui>
#include "qt_window.h"
#include "qt_graphics.h"

C_CAPSULE_START

opp_vtable_extern(xultb_window);
static void (*backup_paint)(struct xultb_window*win, struct xultb_graphics*g);

static void qt_window_impl_paint_wrapper(struct xultb_window*win, struct xultb_graphics*g) {
	QtXulTbWindow*qw = (QtXulTbWindow*)win->platform_data;
    // TODO see if the xultb_graphics is initiated ..
    SYNC_LOG(SYNC_VERB, "Painting list\n");
    backup_paint(win, g);
    QTG_CAPSULE(
    		qw->setPage(qtg->page);
    		qtg->painter->end();
    );
    SYNC_LOG(SYNC_VERB, "Rendering list(TODO: refresh now)\n");
    //opp_callback(g, OPPN_ACTION_GUI_RENDER, ql->canvas);
}

static void qt_window_impl_show(struct xultb_window*win) {
	QtXulTbWindow*qw = (QtXulTbWindow*)win->platform_data;
    xultb_guicore_set_dirty(win);
    xultb_guicore_walk(0); // XXX should I force it to render ??
    SYNC_LOG(SYNC_VERB, "Showing Window ..\n");
    qw->show();
}


int xultb_window_platform_create(struct xultb_window*win) {
	QtXulTbWindow*qw = new QtXulTbWindow();
	win->platform_data = qw;
	return 0;
}

int xultb_window_system_platform_init() {
	backup_paint = vtable_xultb_window.paint;
	vtable_xultb_window.paint = qt_window_impl_paint_wrapper;
	vtable_xultb_window.show = qt_window_impl_show;
	return 0;
}

C_CAPSULE_END

