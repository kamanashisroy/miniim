#ifndef XULTB_TEXT_FORMAT_H
#define XULTB_TEXT_FORMAT_H

#include "core/xultb_decorator.h"
#include "core/xultb_exttypes.h"
#include "ui/core/xultb_font.h"

C_CAPSULE_START

int xultb_wrap_next(xultb_str_t*str, xultb_font_t*font, int pos, int width);

C_CAPSULE_END

#endif // XULTB_TEXT_FORMAT_H
