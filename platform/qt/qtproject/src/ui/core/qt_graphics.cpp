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
#include <QVariant>



class QtXulTbGraphics: public QWidget {
Q_OBJECT
public:
  QtXulTbGraphics(QWidget *parent = 0 ):QWidget( parent ) {
    QSizePolicy policy( QSizePolicy::Preferred, QSizePolicy::Preferred );
    policy.setHeightForWidth( true );
    setSizePolicy( policy );
    pen = new QColor();
    page = new QPixmap(100,100);
    painter = new QPainter(page);
  }
  ~QtXulTbGraphics() {
    if(painter)delete painter;
    if(pen)delete pen;
    if(page)delete page;
  }

public: // implements QWidget method
  int heightForWidth(int width) const {
    return width;
  }
  QSize sizeHint() const {
    return QSize( 100, 100 );
  }
  QSize minimumSizeHint () const {
      return QSize( 100, 100 );
  }

public  Q_SLOTS: // implements QWidget method
  void 	setVisible ( bool ) {
      return;
  }

protected:
    // Event handlers
    void mousePressEvent(QMouseEvent *) {
    }
    void mouseReleaseEvent(QMouseEvent *) {}
    void mouseDoubleClickEvent(QMouseEvent *) {}
    void mouseMoveEvent(QMouseEvent *){}
#ifndef QT_NO_WHEELEVENT
    void wheelEvent(QWheelEvent *){}
#endif
    void keyPressEvent(QKeyEvent *){}
    void keyReleaseEvent(QKeyEvent *){}
    void focusInEvent(QFocusEvent *){}
    void focusOutEvent(QFocusEvent *){}
    void enterEvent(QEvent *){}
    void leaveEvent(QEvent *){}
    void paintEvent(QPaintEvent *){
    //QPainter p( this );
    page->fill(this, 0, 0);
    //p.drawImage(0, 0, *page);
  }
    void moveEvent(QMoveEvent *){}
    void resizeEvent(QResizeEvent *){}
    void closeEvent(QCloseEvent *){}
#ifndef QT_NO_CONTEXTMENU
    void contextMenuEvent(QContextMenuEvent *){}
#endif
#ifndef QT_NO_TABLETEVENT
    void tabletEvent(QTabletEvent *){}
#endif
#ifndef QT_NO_ACTION
    void actionEvent(QActionEvent *){}
#endif

#ifndef QT_NO_DRAGANDDROP
    void dragEnterEvent(QDragEnterEvent *){}
    void dragMoveEvent(QDragMoveEvent *){}
    void dragLeaveEvent(QDragLeaveEvent *){}
    void dropEvent(QDropEvent *){}
#endif

    void showEvent(QShowEvent *){}
    void hideEvent(QHideEvent *){}

#if defined(Q_WS_MAC)
    bool macEvent(EventHandlerCallRef, EventRef){return false;}
#endif
#if defined(Q_WS_WIN)
    bool winEvent(MSG *message, long *result){return false;}
#endif
#if defined(Q_WS_X11)
    bool x11Event(XEvent *){return false;}
#endif
#if defined(Q_WS_QWS)
    bool qwsEvent(QWSEvent *){return false;}
#endif

    // Misc. protected functions
    void changeEvent(QEvent *){}

    void inputMethodEvent(QInputMethodEvent *){}
public:
    QVariant inputMethodQuery(Qt::InputMethodQuery) const {
      return QVariant();
    }

protected:
    bool focusNextPrevChild(bool ) {
        return false;
    }
protected:
    void styleChange(QStyle&) {} // compat
    void enabledChange(bool) {}  // compat
    void paletteChange(const QPalette &) {}  // compat
    void fontChange(const QFont &) {} // compat
    void windowActivationChange(bool) {} // compat
    void languageChange() {} // compat
public:
    QPainter*painter;
    QColor*pen;
private:
    QPixmap*page;
};

C_CAPSULE_START
static struct opp_factory graphics_factory;
#define TO_QT_G(x) (QtXulTbGraphics*)(x+1)
#define QTG_CAPSULE(code) ({QtXulTbGraphics *qtg = TO_QT_G(g); \
    code; \
})

static void qt_impl_draw_image(struct xultb_graphics*g, struct xultb_img*img, int x, int y, int anchor) {
    QtXulTbGraphics*qtg = TO_QT_G(g);
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
    QtXulTbGraphics*qtg = TO_QT_G(g);
    // TODO set font
    //qtg->painter->setFont(font->data);
}

QWidget*xultbCanvas = NULL;

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
        //new (qtg) QtXulTbGraphics();
        new QtXulTbGraphics(NULL);
        xultbCanvas = qtg;
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
        //qtg->~QtXulTbGraphics();
        xultbCanvas = NULL;
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

