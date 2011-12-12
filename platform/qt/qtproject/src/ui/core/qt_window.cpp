
#include "config.h"
#include "core/logger.h"
#include "ui/xultb_guicore.h"
#include "ui/core/xultb_list.h"
#include <QtGui>

C_CAPSULE_START

struct qt_window_impl {
    QGraphicsView*canvas;
    void (*proto_paint)(struct xultb_window*win, struct xultb_graphics*g);
};


extern QWidget*xultbCanvas;

static void qt_window_impl_paint_wrapper(struct xultb_window*win, struct xultb_graphics*g) {
    struct qt_window_impl*ql = (struct qt_window_impl*)win->platform_data;
    // TODO see if the xultb_graphics is initiated ..
    //opp_callback(g, 999, ql->canvas);
    SYNC_LOG(SYNC_VERB, "Painting list\n");
    ql->proto_paint(win, g);
    SYNC_LOG(SYNC_VERB, "Rendering list\n");
    //opp_callback(g, OPPN_ACTION_GUI_RENDER, ql->canvas);
}

static void qt_window_impl_show(struct xultb_window*win) {
    struct qt_window_impl*ql = (struct qt_window_impl*)win->platform_data;
    xultb_guicore_set_dirty(win);
    if(!ql->canvas && !xultbCanvas) {
        return;
    }
    //ql->canvas->show();
    xultbCanvas->show();
}


int xultb_window_platform_create(struct xultb_window*win) {
    struct qt_window_impl*ql = new qt_window_impl();
    //ql->canvas = new QGraphicsView();
    ql->canvas = NULL;
    ql->proto_paint = win->paint;
    win->platform_data = ql;
    win->show = qt_window_impl_show;
    win->paint = qt_window_impl_paint_wrapper;
    return 0;
}

C_CAPSULE_END

