
#include "config.h"
#include "core/logger.h"
#include "ui/xultb_guicore.h"
#include "ui/core/xultb_list.h"
#include <QtGui>
#include "qt_window.h"
#include "qt_graphics.h"

C_CAPSULE_START

static void qt_window_impl_paint_wrapper(struct xultb_window*win, struct xultb_graphics*g) {
	QtXulTbWindow*qw = (QtXulTbWindow*)win->platform_data;
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
	QtXulTbWindow*qw = (QtXulTbWindow*)win->platform_data;
    xultb_guicore_set_dirty(win);
    xultb_guicore_walk(0);
    SYNC_LOG(SYNC_VERB, "Showing Window ..\n");
    qw->show();
}


int xultb_window_platform_create(struct xultb_window*win) {
	QtXulTbWindow*qw = new QtXulTbWindow();
	qw->proto_paint = win->paint;
    win->platform_data = qw;
    win->show = qt_window_impl_show;
    win->paint = qt_window_impl_paint_wrapper;
    return 0;
}

C_CAPSULE_END

