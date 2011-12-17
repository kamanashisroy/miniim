/*
 * qt_graphics.h
 *
 *  Created on: Nov 28, 2011
 *      Author: kamanashisroy
 */

#ifndef QT_GRAPHICS_H_
#define QT_GRAPHICS_H_


#include <QPainter>
#include <QColor>
#include <QGraphicsView>
#include <QPixmap>
#include <QVariant>


class QtXulTbGraphics {
public:
	QtXulTbGraphics() {
		pen = new QColor();
		page = new QPixmap(200,400);
		painter = new QPainter(page);
	}
	~QtXulTbGraphics() {
		if(painter)delete painter;
		if(pen)delete pen;
		if(page)delete page;
	}
public:
	QPainter*painter;
	QColor*pen;
	QPixmap*page;
};

#define TO_QT_G(x) (QtXulTbGraphics*)(x+1)
#define QTG_CAPSULE(code) ({QtXulTbGraphics *qtg = TO_QT_G(g); \
    code \
})

#endif /* QT_GRAPHICS_H_ */
