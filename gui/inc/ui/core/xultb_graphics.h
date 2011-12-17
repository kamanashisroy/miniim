#ifndef XULTB_GRAPHICS_H_
#define XULTB_GRAPHICS_H_

#include "core/config.h"
#include "core/xultb_decorator.h"
#include "core/xultb_exttypes.h"
#include "ui/core/xultb_font.h"
#include "opp/opp_type.h"

C_CAPSULE_START
struct xultb_img;
enum xultb_graphics_anchor {
	XULTB_GRAPHICS_TOP = 1,
	XULTB_GRAPHICS_HCENTER = (1<<1),
	XULTB_GRAPHICS_LEFT = (1 << 2),
	XULTB_GRAPHICS_RIGHT = (1 << 3),
	XULTB_GRAPHICS_BOTTOM = (1 << 4),
};

struct xultb_graphics {
#if 0
	void(*clipRect)(int x, int y, int width, int height);
	void(*copyArea)(int x_src, int y_src, int width, int height, int x_dest, int y_dest, int anchor);
	void(*drawArc)(int x, int y, int width, int height, int startAngle, int arcAngle);
	void(*drawChar)(char character, int x, int y, int anchor);
	void(*drawChars)(xultb_str_t*data, int offset, int length, int x, int y, int anchor);
#endif
	void(*draw_image)(struct xultb_graphics*g, struct xultb_img*img, int x, int y, int anchor);
	void(*draw_line)(struct xultb_graphics*g, int x1, int y1, int x2, int y2);
	void(*draw_rect)(struct xultb_graphics*g, int x, int y, int width, int height);
#if 0
	void(*drawRegion)(struct xultb_img*src, int x_src, int y_src, int width, int height, int transform, int x_dest, int y_dest, int anchor);
	void(*drawRGB)(int *rgbData, int offset, int scanlength, int x, int y, int width, int height, boolean processAlpha);
#endif
	void(*draw_round_rect)(struct xultb_graphics*g, int x, int y, int width, int height, int arcWidth, int arcHeight);
	void(*draw_string)(struct xultb_graphics*g, xultb_str_t*str, int x, int y, int width, int height, int anchor);
#if 0
	void(*drawSubstring)(String str, int offset, int len, int x, int y, int anchor);
	void(*fillArc)(int x, int y, int width, int height, int startAngle, int arcAngle);
#endif
	void(*fill_rect)(struct xultb_graphics*g, int x, int y, int width, int height);
	void(*fill_round_rect)(struct xultb_graphics*g, int x, int y, int width, int height, int arcWidth, int arcHeight);
	void(*fill_triangle)(struct xultb_graphics*g, int x1, int y1, int x2, int y2, int x3, int y3);
#if 0
	int(*getBlueComponent)();
	int(*getClipHeight)();
	int(*getClipWidth)();
	int(*getClipX)();
	int(*getClipY)();
	int(*getColor)();
	int(*getDisplayColor)(int color);
	Font(*getFont)();
	int(*getGrayScale)();
	int(*getGreenComponent)();
	int(*getRedComponent)();
	int(*getStrokeStyle)();
	int(*getTranslateX)();
	int(*getTranslateY)();
	void(*setClip)(int x, int y, int width, int height);
#endif
	void(*set_color)(struct xultb_graphics*g, int rgb);
#if 0
	void(*setColor_full)(int red, int green, int blue);
#endif
	void(*set_font)(struct xultb_graphics*g, xultb_font_t*font);
#if 0
	void(*setGrayScale)(int value);
	void(*setStrokeStyle)(int style);
	void(*translate)(int x, int y);
#endif
};

//opp_type_new(xultb_graphics);

int xultb_graphics_system_init();
#define xultb_graphics_create xultb_graphics_platform_create
struct xultb_graphics*xultb_graphics_platform_create();

C_CAPSULE_END

#endif /* XULTB_GRAPHICS_H_ */
