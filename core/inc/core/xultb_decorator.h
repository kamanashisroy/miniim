#ifndef XULTB_DECORATOR_H
#define XULTB_DECORATOR_H

#define XULTB_CORE_UNIMPLEMENTED() assert(0)

#ifdef __cplusplus
#define C_CAPSULE_START extern "C" {
#define C_CAPSULE_END }
//#define ST_ASS(x,y) x:y
#else
#define C_CAPSULE_START
#define C_CAPSULE_END
//#define ST_ASS(x,y) .x=y
#endif

#endif
