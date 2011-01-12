

#include "core/xultb_exttypes.h"
#include "ui/core/xultb_text_format.h"

int xultb_wrap_next(xmltb_str_t*str, xultb_font_t*font, int pos, int width) {
	int i = pos,start = pos;
	if(str == NULL || font == NULL || width <= 0 ) {
	  return -1;
	}
	int len = str->len;
	if(pos == len) {
	  return -1;
	}
	while (true) {
		while (i < len && str->str[i] > ' ')i++;
		int w = font->substring_width(font, str, start, i - start);
		if (pos == start) {
			  if (w > width) {
				  while (font->substring_width (font, str, start, (--i - start )) > width);
				  pos = i;
				  break;
			  }
		  }

		  if (w <= width) pos = i;

		if (w > width || i >= len || str.str[i] == '\n') break;
		i++;
	}
	pos = pos >= len ? pos : ++pos;
	return pos;
}

