/*
 * qt_graphics.cpp
 *
 *  Created on: Jan 21, 2011
 *      Author: ayaskanti
 */

#include "ui/xultb_guicore.h"
#include "ui/core/xultb_graphics.h"
#include "qt_graphics.h"
//#include <QtCore>
#include <QPainter>
#include <QColor>


C_CAPSULE_START


struct qt_graphics {
    QPainter*painter;
    QColor*pen;
};

static struct opp_factory graphics_factory;

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

static void xultb_draw_image(struct xultb_graphics*g, struct xultb_img*img, int x, int y, int anchor) {
    struct qt_graphics*qtg = (struct qt_graphics*)(g+1);
    // TODO create qimage
    //qtg->painter->drawImage(x, y, img->data);
}

static void xultb_draw_line(struct xultb_graphics*g, int x1, int y1, int x2, int y2) {
    struct qt_graphics*qtg = (struct qt_graphics*)(g+1);
    qtg->painter->drawLine(x1, y1, x2, y2);
}

static void xultb_draw_rect(struct xultb_graphics*g, int x, int y, int width, int height) {
    struct qt_graphics*qtg = (struct qt_graphics*)(g+1);
    qtg->painter->drawRect(x, y, width, height);
}

static void xultb_draw_round_rect(struct xultb_graphics*g, int x, int y, int width, int height, int arcWidth, int arcHeight) {
    struct qt_graphics*qtg = (struct qt_graphics*)(g+1);
    qtg->painter->drawRoundedRect(x, y, width, height, arcWidth, arcHeight,  Qt::RelativeSize);
}

static void xultb_draw_string(struct xultb_graphics*g, xultb_str_t*str, int x, int y, int anchor) {
    struct qt_graphics*qtg = (struct qt_graphics*)(g+1);
    QString text = new QString(str->str, str->len);
	qtg->painter->drawText(x, y, text);
	delete text;
}

static void xultb_fill_rect(struct xultb_graphics*g, int x, int y, int width, int height) {
    struct qt_graphics*qtg = (struct qt_graphics*)(g+1);
    qtg->painter->fillRect(x, y, width, height, NULL);
}

static void xultb_fill_triangle(struct xultb_graphics*g, int x1, int y1, int x2, int y2, int x3, int y3) {
    struct qt_graphics*qtg = (struct qt_graphics*)(g+1);
    // TODO fill me
	qtg->painter->fillPath(qtg->path, NULL);
}

static void xultb_fill_round_rect(struct xultb_graphics*g, int x, int y, int width, int height, int arcWidth, int arcHeight) {
    struct qt_graphics*qtg = (struct qt_graphics*)(g+1);
    qtg->painter->fillRect(x, y, width, height, NULL);
}

static void xultb_set_color(struct xultb_graphics*g, int rgb) {
    struct qt_graphics*qtg = (struct qt_graphics*)(g+1);
    qtg->pen->setRgb(rgb);
	qtg->painter->setPen(qtg->pen);
}

static void xultb_set_font(struct xultb_graphics*g, xultb_font_t*font) {
    struct qt_graphics*qtg = (struct qt_graphics*)(g+1);
    // TODO set font
    //qtg->painter->setFont(font->data);
}

OPP_CB(xultb_graphics) {
    struct xultb_graphics*g = (struct xultb_graphics*)data;
    struct qt_graphics*qtg = (struct qt_graphics*)g+1;
	switch(callback) {
	case OPPN_ACTION_INITIALIZE:
		g->draw_image = xultb_draw_image;
		g->draw_line = xultb_draw_line;
		g->draw_rect = xultb_draw_rect;
		g->draw_round_rect = xultb_draw_round_rect;
		g->draw_string = xultb_draw_string;
		g->fill_rect = xultb_fill_rect;
		g->fill_round_rect = xultb_fill_round_rect;
		g->fill_triangle = xultb_fill_triangle;
		g->set_color = xultb_set_color;
		g->set_font = xultb_set_font;
        qtg->painter = new QPainter();
        qtg->pen = new QColor();
		return 0;
	case OPPN_ACTION_FINALIZE:
        delete qtg->painter;
        delete qtg->pen;
		break;
	}
	return 0;
}

struct xultb_graphics*xultb_graphics_create() {
    struct xultb_graphics*g = (struct xultb_graphics*)OPP_ALLOC2(&graphics_factory, NULL);
	if(!g) {
		return NULL;
	}
	return g;
}

int xultb_list_item_system_init() {
	SYNC_ASSERT(OPP_FACTORY_CREATE(&graphics_factory, 1
			,sizeof(struct xultb_graphics)+sizeof(struct qt_graphics)
			,OPP_CB_FUNC(xultb_graphics)) == 0);
	return 0;
}

C_CAPSULE_END
