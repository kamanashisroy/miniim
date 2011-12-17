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
#include "qt_font.h"
//#include <QtCore>
#include <QPainter>
#include <QColor>
#include <QGraphicsView>
#include <QPixmap>
#include <QVariant>

C_CAPSULE_START
static struct opp_factory graphics_factory;

static void qt_impl_draw_image(struct xultb_graphics*g, struct xultb_img*img, int x, int y, int anchor) {
	QTG_CAPSULE(
//			qtg->painter->drawImage(x, y, img->data);
	);
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

static void qt_impl_draw_string(struct xultb_graphics*g, xultb_str_t*str, int x, int y, int width, int height, int anchor) {
    QTG_CAPSULE(
	SYNC_LOG(SYNC_VERB, "Drawing string %s\n", str->str);
    QString text(str->str);
    int flags = 0;

    if(anchor & XULTB_GRAPHICS_TOP) {
    	flags |= Qt::AlignTop;
    }
    if(anchor & XULTB_GRAPHICS_LEFT) {
    	flags |= Qt::AlignLeft;
    }
    if(anchor & XULTB_GRAPHICS_RIGHT) {
		flags |= Qt::AlignRight;
	}
    if(anchor & XULTB_GRAPHICS_HCENTER) {
    	flags |= Qt::AlignHCenter;
    }
    if(anchor & XULTB_GRAPHICS_BOTTOM) {
    	flags |= Qt::AlignBottom;
    }
	qtg->painter->drawText(x, y, width, height, flags, text);
//    	qtg->painter->drawText(x, y, text);
    );
}

static void qt_impl_fill_rect(struct xultb_graphics*g, int x, int y, int width, int height) {
    QTG_CAPSULE(
    qtg->painter->fillRect(x, y, width, height, *qtg->pen);
    );
}

static void qt_impl_fill_triangle(struct xultb_graphics*g, int x1, int y1, int x2, int y2, int x3, int y3) {
    QtXulTbGraphics*qtg = TO_QT_G(g);
    // TODO fill me
    //qtg->painter->fillPath(qtg->path, NULL);
}

static void qt_impl_fill_round_rect(struct xultb_graphics*g, int x, int y, int width, int height, int arcWidth, int arcHeight) {
    QtXulTbGraphics*qtg = TO_QT_G(g);
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
    SYNC_ASSERT(font);
    QTG_CAPSULE(
    QtXulTbFont*qtfont = TO_QT_FONT(font);
    qtg->painter->setFont(qtfont->font);
	);
}

OPP_CB(qt_impl_graphics) {
    struct xultb_graphics*g = (struct xultb_graphics*)data;
    QtXulTbGraphics*qtg = TO_QT_G(g);
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
        new (qtg) QtXulTbGraphics();
        return 0;
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
        qtg->~QtXulTbGraphics();
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
            ,sizeof(struct xultb_graphics)+sizeof(QtXulTbGraphics)
            ,OPP_CB_FUNC(qt_impl_graphics)) == 0);
	return 0;
}

C_CAPSULE_END

