
#include "config.h"
#include "core/logger.h"
#include "ui/xultb_guicore.h"
#include "ui/core/xultb_list.h"
#include <QtGui>
#include <QFont>
#include <QFontMetrics>
#include "qt_font.h"
#include "ui/core/xultb_font.h"

C_CAPSULE_START

static struct opp_factory font_factory;

static int qt_impl_get_height(struct xultb_font*font) {
    QtXulTbFont*qtfont = TO_QT_FONT(font);
    return qtfont->metrics.height();
}

static int qt_impl_substring_width(struct xultb_font*font, xultb_str_t*str, int offset, int width) {
    QtXulTbFont*qtfont = TO_QT_FONT(font);
    QString text(str->str+offset);
    return qtfont->metrics.width(text, width);
}

static struct xultb_font*some_default_font = NULL;
struct xultb_font*xultb_font_get(int UNUSED_VAR(face), int UNUSED_VAR(style), int UNUSED_VAR(size)) {
	if(!some_default_font) {
		some_default_font = xultb_font_create();
	}
	return some_default_font;
}

int xultb_font_get_face(struct xultb_font*UNUSED_VAR(font)) {
	// TODO implement me
	return 0;
}
int xultb_font_get_style(struct xultb_font*UNUSED_VAR(font)) {
	// TODO implement me
	return 0;
}
int xultb_font_get_size(struct xultb_font*UNUSED_VAR(font)) {
	// TODO implement me
	return 0;
}


OPP_CB(qt_impl_font) {
    struct xultb_font*font = (struct xultb_font*)data;
    QtXulTbFont*qtfont = TO_QT_FONT(font);
    switch(callback) {
    case OPPN_ACTION_INITIALIZE:
        font->get_height = qt_impl_get_height;
        font->substring_width = qt_impl_substring_width;
        new (qtfont) QtXulTbFont();
        return 0;
    case OPPN_ACTION_VIEW:
        SYNC_LOG(SYNC_VERB, "Allocated at %p\n", data);
        break;
    case OPPN_ACTION_FINALIZE:
        qtfont->~QtXulTbFont();
        break;
    }
    return 0;
}

struct xultb_font*xultb_font_platform_create() {
    struct xultb_font*font = (struct xultb_font*)OPP_ALLOC2(&font_factory, NULL);
    return font;
}

int xultb_font_system_init() {
    SYNC_ASSERT(OPP_FACTORY_CREATE(&font_factory, 1
            ,sizeof(struct xultb_font)+sizeof(QtXulTbFont)
            ,OPP_CB_FUNC(qt_impl_font)) == 0);
    return 0;
}
C_CAPSULE_END

