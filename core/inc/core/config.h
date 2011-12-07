#ifndef XULTB_CORE_CONFIG_H
#define XULTB_CORE_CONFIG_H

#define COMPONENT_SCALABILITY 2

#ifdef  QTGUI_LIBRARY
#include "qt_config.h"
#else
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>
#include <stdarg.h>

typedef uint8_t SYNC_UWORD8_T;
typedef uint16_t SYNC_UWORD16_T;
typedef uint32_t SYNC_UWORD32_T;

typedef int8_t SYNC_SWORD8_T;
typedef int16_t SYNC_SWORD16_T;
typedef int32_t SYNC_SWORD32_T;

#define SYNC_ASSERT(x) assert(x)
#endif

#include "core/xultb_decorator.h"

#endif //XULTB_CONFIG_H
