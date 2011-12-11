/*
 * qt_graphics.cpp
 *
 *  Created on: Jan 21, 2011
 *      Author: ayaskanti
 */

#include "core/logger.h"
#include "ui/xultb_guicore.h"
#include "ui/core/xultb_graphics.h"
#include "qt_graphics.h"
//#include <QtCore>
#include <QPainter>
#include <QColor>
#include <QGraphicsView>
#include <QPixmap>


C_CAPSULE_START


struct qt_graphics {
    QPainter*painter;
    QColor*pen;
    QWidget*canvas;
};

#define TO_QT_G(x) (struct qt_graphics*)(x+1)
static struct opp_factory graphics_factory;

C_CAPSULE_END

class QtXulTbGraphics: public QWidget {
private:
    QPainter*painter;
    QColor*pen;
};

#if 0
void(*draw_image)(struct xultb_graphics*g, struct xultb_img*img, int x, int y, int anchor);
void(*draw_line)(struct xultb_graphics*g, int x1, int y1, int x2, int y2);
void(*draw_rect)(struct xultb_graphics*g, int x, int y, int width, int height);
void(*draw_round_rect)(struct xultb_graphics*g, int x, int y, int width, int height, int arcWidth, int arcHeight);
void(*draw_string)(struct xultb_graphics*g, xultb_str_t*str, int x, int y, int anchor);
void(*fill_rect)(struct xultb_graphics*g, int x, int y, int width, int height);
void(*fill_round_rect)(struct xultb_graphics*g, int x, int y, int width, int height, int arcWidth, int arcHeight);
void(*fill_triangle)(struct xultb_graphics*g, int x1, int y1, int x2, int y2, int x3, int y3);
void(*set_color)(struct xultb_graphics*g, int rgb);
void(*set_font)(struct xultb_graphics*g, xultb_font_t*font);
#endif

#define QTG_CAPSULE(code) ({struct qt_graphics *qtg = TO_QT_G(g); \
    if(!qtg->canvas) { \
        SYNC_LOG(SYNC_VERB, "Could not draw canvas\n"); \
        return; \
    } \
    qtg->painter->begin(qtg->canvas); \
    code; \
    qtg->painter->end(); \
})

static void qt_impl_draw_image(struct xultb_graphics*g, struct xultb_img*img, int x, int y, int anchor) {
    struct qt_graphics*qtg = TO_QT_G(g);
    // TODO create qimage
    //qtg->painter->drawImage(x, y, img->data);
}

static void qt_impl_draw_line(struct xultb_graphics*g, int x1, int y1, int x2, int y2) {
    QTG_CAPSULE(
    qtg->painter->drawLine(x1, y1, x2, y2);
    );
}

static void qt_impl_draw_rect(struct xultb_graphics*g, int x, int y, int width, int height) {
    QTG_CAPSULE(
    qtg->painter->drawRect(x, y, width, height);
    );
}

static void qt_impl_draw_round_rect(struct xultb_graphics*g, int x, int y, int width, int height, int arcWidth, int arcHeight) {
    QTG_CAPSULE(
    qtg->painter->drawRoundedRect(x, y, width, height, arcWidth, arcHeight,  Qt::RelativeSize);
    );
}

static void qt_impl_draw_string(struct xultb_graphics*g, xultb_str_t*str, int x, int y, int anchor) {
    QTG_CAPSULE(
    QString*text = new QString(str->str);
    qtg->painter->drawText(x, y, *text);
	delete text;
    );
}

static void qt_impl_fill_rect(struct xultb_graphics*g, int x, int y, int width, int height) {
    QTG_CAPSULE(
    qtg->painter->fillRect(x, y, width, height, *qtg->pen);
    );
}

static void qt_impl_fill_triangle(struct xultb_graphics*g, int x1, int y1, int x2, int y2, int x3, int y3) {
    struct qt_graphics*qtg = TO_QT_G(g);
    // TODO fill me
    //qtg->painter->fillPath(qtg->path, NULL);
}

static void qt_impl_fill_round_rect(struct xultb_graphics*g, int x, int y, int width, int height, int arcWidth, int arcHeight) {
    struct qt_graphics*qtg = TO_QT_G(g);
    QTG_CAPSULE(
    qtg->painter->fillRect(x, y, width, height, *qtg->pen);
    );
}

static void qt_impl_set_color(struct xultb_graphics*g, int rgb) {
    QTG_CAPSULE(
    //SYNC_LOG_OPP(&graphics_factory);
    //opp_callback2(g, OPPN_ACTION_VIEW, NULL);
    qtg->pen->setRgb(rgb);
    qtg->painter->setPen(*qtg->pen);
    );
}

static void qt_impl_set_font(struct xultb_graphics*g, xultb_font_t*font) {
    struct qt_graphics*qtg = TO_QT_G(g);
    // TODO set font
    //qtg->painter->setFont(font->data);
}

OPP_CB(qt_impl_graphics) {
    struct xultb_graphics*g = (struct xultb_graphics*)data;
    struct qt_graphics*qtg = TO_QT_G(g);
	switch(callback) {
	case OPPN_ACTION_INITIALIZE:
        g->draw_image = qt_impl_draw_image;
        g->draw_line = qt_impl_draw_line;
        g->draw_rect = qt_impl_draw_rect;
        g->draw_round_rect = qt_impl_draw_round_rect;
        g->draw_string = qt_impl_draw_string;
        g->fill_rect = qt_impl_fill_rect;
        g->fill_round_rect = qt_impl_fill_round_rect;
        g->fill_triangle = qt_impl_fill_triangle;
        g->set_color = qt_impl_set_color;
        g->set_font = qt_impl_set_font;
        qtg->painter = NULL;
        qtg->pen = new QColor();
        return 0;
	case 999:
		if(!qtg->painter) {
            qtg->canvas = (QGraphicsView*)cb_data;
            qtg->painter = new QPainter(qtg->canvas);
            SYNC_LOG(SYNC_VERB, "Canvas is set\n");
        }
		break;
    case OPPN_ACTION_GUI_RENDER:
    {
        QGraphicsView*canvas = (QGraphicsView*)cb_data;
        canvas->render(qtg->painter);
        SYNC_LOG(SYNC_VERB, "Rendered canvas\n");
        return 0;
    }
    case OPPN_ACTION_VIEW:
        SYNC_LOG(SYNC_VERB, "Allocated at %p\n", data);
        break;
	case OPPN_ACTION_FINALIZE:
        if(qtg->painter)delete qtg->painter;
        if(qtg->pen)delete qtg->pen;
		break;
	}
	return 0;
}

struct xultb_graphics*xultb_graphics_platform_create() {
    struct xultb_graphics*g = (struct xultb_graphics*)OPP_ALLOC2(&graphics_factory, NULL);
	if(!g) {
		return NULL;
	}
	return g;
}

int xultb_graphics_system_init() {
	SYNC_ASSERT(OPP_FACTORY_CREATE(&graphics_factory, 1
            ,sizeof(struct xultb_graphics)+sizeof(struct qt_graphics)
            ,OPP_CB_FUNC(qt_impl_graphics)) == 0);
	return 0;
}

//C_CAPSULE_END
