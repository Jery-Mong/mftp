#ifndef __THREAD_POOL_H
#define __THREAD_POOL_H

#include <pthread.h>
#include "list.h"

typedef struct work {
	void (*func)(void *);
	void *arg;
} work_t;

typedef struct thread_pool {
	pthread_t *thr_array;
	int tp_size;
	
	int curr_work_size;
	int max_work_size;
	list_t work_queue;
	
	pthread_mutex_t wlock;
	pthread_cond_t getcond;
	pthread_cond_t addcond;
} thread_pool_t;

void thread_pool_init(thread_pool_t *, int, int);
void thread_pool_add(thread_pool_t *, void (*func)(void *), void *);
void thread_pool_destroy(thread_pool_t *);

#endif
