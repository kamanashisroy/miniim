#ifndef XULTB_GRAPHICS_H_
#define XULTB_GRAPHICS_H_

#include "core/xultb_decorator.h"
#include "core/xultb_exttypes.h"
#include "ui/core/xultb_font.h"

enum xultb_graphics_anchor {
	XULTB_GRAPHICS_TOP = 0,
	XULTB_GRAPHICS_HCENTER,
	XULTB_GRAPHICS_LEFT,
};

struct xultb_graphics {
#if 0
	void(*clipRect)(int x, int y, int width, int height);
	void(*copyArea)(int x_src, int y_src, int width, int height, int x_dest, int y_dest, int anchor);
	void(*drawArc)(int x, int y, int width, int height, int startAngle, int arcAngle);
	void(*drawChar)(char character, int x, int y, int anchor);
	void(*drawChars)(xultb_str_t*data, int offset, int length, int x, int y, int anchor);
	void(*drawImage)(Image img, int x, int y, int anchor);
#endif
	void(*draw_line)(struct xultb_graphics*g, int x1, int y1, int x2, int y2);
#if 0
	void(*drawRect)(int x, int y, int width, int height);
	void(*drawRegion)(struct xultb_img*src, int x_src, int y_src, int width, int height, int transform, int x_dest, int y_dest, int anchor);
	void(*drawRGB)(int *rgbData, int offset, int scanlength, int x, int y, int width, int height, boolean processAlpha);
	void(*drawRoundRect)(int x, int y, int width, int height, int arcWidth, int arcHeight);
#endif
	void(*draw_string)(struct xultb_graphics*g, xultb_str_t*str, int x, int y, int anchor);
#if 0
	void(*drawSubstring)(String str, int offset, int len, int x, int y, int anchor);
	void(*fillArc)(int x, int y, int width, int height, int startAngle, int arcAngle);
#endif
	void(*fill_rect)(struct xultb_graphics*g, int x, int y, int width, int height);
#if 0
	void(*fillRoundRect)(int x, int y, int width, int height, int arcWidth, int arcHeight);
	void(*fillTriangle)(int x1, int y1, int x2, int y2, int x3, int y3);
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
	void(*set_color)(struct xultb_graphics*g, int RGB);
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

#endif /* XULTB_GRAPHICS_H_ */
