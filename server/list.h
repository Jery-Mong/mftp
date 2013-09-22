#ifndef __LIST_H
#define __LIST_H

struct list_node {
	void *data;
	struct list_node *prev;
	struct list_node *next;
	
};
typedef struct list_node node_t;

struct list {
	int node_cnt;
	node_t *head;
};
typedef struct list list_t;



#define for_each_node(iter, list) for(iter = (list)->head->next; iter != (list)->head ; iter = iter->next)
 
#define list_size(list) ((list)->node_cnt)
#define list_is_head(list, node) (((list)->head->next == node) ? 1 : 0)
#define list_is_tail(list, node) (((list)->head->prev == node) ? 1 : 0)

void list_init(list_t *);
void list_destroy(list_t *);
void *list_del_next(list_t *, void *);
void *list_del_after(list_t *, void *);
int list_insert_next(list_t *, void *, void *);
int list_insert_after(list_t *, void *, void *);
int list_del_node(list_t *, void *);
int list_insert_tail(list_t *, void*);
int list_insert_head(list_t *, void*);
void *list_del_tail(list_t *);
void *list_del_head(list_t *);
int list_sort_insert(list_t *, void *, int (*)(void *, void *));

#endif
