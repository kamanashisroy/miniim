
//#define OBJ_BUFFER_HAS_LOCK
#ifdef OBJ_BUFFER_HAS_LOCK
#include <pthread.h>
#define obj_factory_mutex_t pthread_mutex_t
#define obj_factory_mutex_lock pthread_mutex_lock
#define obj_factory_mutex_trylock pthread_mutex_trylock
#define obj_factory_mutex_unlock pthread_mutex_unlock
#define obj_factory_mutex_init phtread_mutex_init
#define obj_factory_mutex_destroy pthread_mutex_destroy
#endif


