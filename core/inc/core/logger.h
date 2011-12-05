/*
 * logger.h
 *
 *  Created on: Dec 28, 2010
 *      Author: kgm212
 */

#ifndef SYNC_LOGGER_H_
#define SYNC_LOGGER_H_

#include "core/config.h"
//#include "types_internal.h"

C_CAPSULE_START

enum sync_logtype {
	SYNC_DEBUG,
	SYNC_VERB,
	SYNC_NOTICE,
	SYNC_WARNING,
	SYNC_ERROR,
};

C_CAPSULE_END

#ifdef NO_LOG
#define SYNC_LOG_INIT()
#define SYNC_LOG_DEINIT()
#define SYNC_DEBUG_VERB(x, ...)

#define SYNC_LOG(y, ...)
#define SYNC_LOG_OPP(x)
#else
#ifdef ASTERISK_CHANNEL
#include "ast_logger.h"
#else
#ifdef SYMBIAN_CLIENT
#include "symb_logger.h"
#else
#ifdef QT
#include "qt_logger.h"
#else
#include "linux_logger.h"
#endif // QT
#endif // SYMBIAN_CLIENT
#endif // ASTERISK_CHANNEL
#endif

#endif /* SYNC_LOGGER_H_ */
