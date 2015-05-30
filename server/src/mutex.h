#include <pthread.h>

#define mutex_t pthread_mutex_t
#define pmutex_t pthread_mutex_t*

#define mutex_initer() PTHREAD_MUTEX_INITIALIZER

#define mutex_create() (pthread_mutex_t*)malloc(sizeof(szieof(pthread_mutex_t)))

#define mutex_init(lock, attr) pthread_mutex_init(lock, attr)

#define mutex_destroy(lock) pthread_mutex_destory(lock)

#define mutex_lock(mutex) pthread_mutex_lock(mutex)

#define mutex_unlock(mutex) pthread_mutex_unlock(mutex)

#define mutex_destroy(lock)\
