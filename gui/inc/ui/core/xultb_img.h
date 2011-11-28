#ifndef XULTB_IMG_H
#define XULTB_IMG_H

#include "core/config.h"

C_CAPSULE_START

struct xultb_img {
   int height;
   int width;
   void*data;
};

typedef struct xultb_img xultb_img_t;

C_CAPSULE_END

#endif // XULTB_IMG_H
