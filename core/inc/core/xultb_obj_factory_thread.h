#ifndef XULTB_OBJ_FACTORY_THREAD_H
#define XULTB_OBJ_FACTORY_THREAD_H

#ifdef TEST_OBJ_FACTORY_UTILS
//#define OBJ_BUFFER_HAS_LOCK
#endif
#ifdef OBJ_BUFFER_HAS_LOCK
#include <pthread.h>
#define xultb_mutex_t pthread_mutex_t
#define xultb_mutex_lock pthread_mutex_lock
#define xultb_mutex_trylock pthread_mutex_trylock
#define xultb_mutex_unlock pthread_mutex_unlock
#define xultb_mutex_init phtread_mutex_init
#define xultb_mutex_destroy pthread_mutex_destroy
#endif


#endif // XULTB_OBJ_FACTORY_THREAD_H

