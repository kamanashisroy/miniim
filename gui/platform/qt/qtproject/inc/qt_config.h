#ifndef XULTB_QT_CONFIG_H
#define XULTB_QT_CONFIG_H

#include "QtCore/qglobal.h"
//#include "QtCore/qobject.h"
//#include <stdio.h>
//#include <stdlib.h>
//#include <sys/types.h>
//#include <string.h>
//#include <time.h>
//#include <errno.h>
//#include <assert.h>
//#include <math.h>
//#include <fcntl.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <sys/time.h>
//#include <sys/ioctl.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>

typedef quint8 SYNC_UWORD8_T;
typedef quint16 SYNC_UWORD16_T;
typedef quint32 SYNC_UWORD32_T;

typedef qint16 SYNC_SWORD16_T;
typedef qint32 SYNC_SWORD32_T;

#include "unistd.h"
#include "string.h"
#include "stdlib.h"
#include <assert.h>
#define SYNC_ASSERT(x) assert(x)

#ifdef __cplusplus
}
#endif

#endif //XULTB_QT_CONFIG_H

