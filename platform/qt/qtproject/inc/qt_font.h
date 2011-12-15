#ifndef QT_WINDOW_H
#define QT_WINDOW_H

#include <QFont>
#include <QFontMetrics>

class QtXulTbFont {
public:
    QtXulTbFont():metrics(font) {
        //new (&font) QFont();
        //new (&metrics) QFontMetrics(font);
    }
    ~QtXulTbFont() {
    }
public:
    QFont font;
    QFontMetrics metrics;
};

#define TO_QT_FONT(x) (QtXulTbFont*)(x+1)
#define QTFONT_CAPSULE(x,code) ({QtXulTbFont *qtfont = TO_QT_FONT(x); \
    code \
})


#endif // QT_WINDOW_H
