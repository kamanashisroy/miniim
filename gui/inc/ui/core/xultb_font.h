#ifndef XULTB_FONT_H
#define XULTB_FONT_H

#include "core/config.h"
#include "core/xultb_exttypes.h"

C_CAPSULE_START

enum xultb_font_style {
	XULTB_FONT_STYLE_UNDERLINED = 1,
	XULTB_FONT_STYLE_BOLD = (1 << 1),
	XULTB_FONT_STYLE_ITALIC = (1 << 2),
	XULTB_FONT_STYLE_PLAIN = (1 << 3),
};

enum xultb_font_size {
	XULTB_FONT_SIZE_SMALL = 1,
	XULTB_FONT_SIZE_MEDIUM = 2,
	XULTB_FONT_SIZE_LARGE = 3,
};

enum xultb_font_face {
	XULTB_FONT_FACE_DEFAULT = 1,
	XULTB_FONT_FACE_SYSTEM = 2,
};

struct xultb_font {
   int (*get_height)(struct xultb_font*font);
   int (*substring_width)(struct xultb_font*font, xultb_str_t*str, int offset, int width);
};

typedef struct xultb_font xultb_font_t;

#define xultb_font_create xultb_font_platform_create
struct xultb_font*xultb_font_platform_create();
struct xultb_font*xultb_font_get(int face, int style, int size);
int xultb_font_get_face(struct xultb_font*font);
int xultb_font_get_style(struct xultb_font*font);
int xultb_font_get_size(struct xultb_font*font);
int xultb_font_system_init();

C_CAPSULE_END

#endif // XULTB_FONT_H
