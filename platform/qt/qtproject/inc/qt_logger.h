/*
 * qt_logger.h
 *
 *  Created on: Jun 29, 2011
 *      Author: root
 */

#ifndef QT_LOGGER_H_
#define QT_LOGGER_H_

#include <stdio.h>
C_CAPSULE_START
//#define SYNC_LOG_INIT()
//#define SYNC_LOG_DEINIT()
//#define SYNC_DEBUG_VERB(x, ...)
//#define SYNC_LOG(y, ...)
#define SYNC_LOG_INIT()
#define SYNC_LOG_DEINIT()
#define SYNC_DEBUG_VERB(x, ...) do { \
	if(syncsystem_conf.syn_debug) printf("  " __VA_ARGS__); \
}while(0)

#define SYNC_LOG(y, ...) do { \
	if(y==SYNC_DEBUG) \
		printf("--" __VA_ARGS__); \
	else if(y==SYNC_VERB) \
		printf("  ==" __VA_ARGS__); \
	else if(y==SYNC_NOTICE) \
		printf("    Notice: " __VA_ARGS__); \
	else if(y==SYNC_WARNING) \
		printf("    Warning: " __VA_ARGS__); \
	else \
		printf("    Error: " __VA_ARGS__); \
}while(0)

#define SYNC_LOG_OPP(x) opp_factory_verb(x, NULL, NULL, xultb_log_helper, NULL)

void xultb_log_helper(void*fdptr, const char *fmt, ...);

C_CAPSULE_END

#endif /* QT_LOGGER_H_ */
