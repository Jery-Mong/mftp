#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "thread_pool.h"
#include "list.h"

void *get_work(void *data)
{
	thread_pool_t *tp = (thread_pool_t *)data;

	while(1) {
		pthread_mutex_lock(&tp->wlock);
		while (tp->curr_work_size == 0)
			pthread_cond_wait(&tp->getcond, &tp->wlock);
		
		work_t *wk = (work_t *)list_del_head(&tp->work_queue);
		
		tp->curr_work_size--;
		pthread_cond_signal(&tp->addcond);
		pthread_mutex_unlock(&tp->wlock);
		
		wk->func(wk->arg);
	}
	return NULL;
}

void thread_pool_init(thread_pool_t *tp, int tp_size, int max_work_size)
{
	memset(tp, 0, sizeof(thread_pool_t));
	
	tp->tp_size = tp_size;
	tp->max_work_size = max_work_size;
	tp->curr_work_size = 0;
	
	tp->thr_array = (pthread_t *)malloc(tp_size * sizeof(pthread_t));

	list_init(&tp->work_queue);
	pthread_mutex_init(&tp->wlock, NULL);
	pthread_cond_init(&tp->getcond, NULL);
	pthread_cond_init(&tp->addcond, NULL);
	
	int i;
	for (i = 0; i < tp_size; i++) {
		pthread_create(&(tp->thr_array[i]), NULL, get_work, tp);
	}
}

void thread_pool_add(thread_pool_t *tp, void (*func)(void *), void *arg)
{
	if (func == NULL)
		return;
	
	pthread_mutex_lock(&tp->wlock);
	while (tp->curr_work_size == tp->max_work_size)
		pthread_cond_wait(&tp->addcond, &tp->wlock);

	work_t *wk = (work_t *)malloc(sizeof(work_t));
	wk->func = func;
	wk->arg = arg;
	
	list_insert_tail(&tp->work_queue, wk);
	tp->curr_work_size++;

	pthread_cond_signal(&tp->getcond);
	pthread_mutex_unlock(&tp->wlock);		
}
/*
void thread_pool_destroy(thread_pool_t *tp)
{
	while (tp->cur_work_size > 0);


	.......;
	

	int i;
	for (i = 0; i < tp->tp_size; i++) {
		pthread_cancel(tp->thr_array[i]);
	}
	free(tp->thr_array);
}
*/
