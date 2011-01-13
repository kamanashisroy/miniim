#ifndef XULTB_FONT_H
#define XULTB_FONT_H

struct xultb_font {
   int (*get_height)(struct xultb_font*font);
   int (*substring_width)(struct xultb_font*font, xultb_str_t*str, int offset, int width);
};

typedef struct xultb_font xultb_font_t;

#endif // XULTB_FONT_H
