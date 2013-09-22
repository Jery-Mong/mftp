#include <stdio.h>
#include <stdlib.h>

#include "list.h"

void list_init(list_t *lst)
{
	lst->node_cnt = 0;
	lst->head = (node_t *)malloc(sizeof(node_t));
	lst->head->next = lst->head;
	lst->head->prev  = lst->head;
}
void list_destroy(list_t *lst)
{
	node_t *iter;
	for_each_node(iter, lst) {
		free(iter->data);
		free(iter);
	}

	free(lst->head);
}


void *list_del_next(list_t *lst, void *data)
{
	void *tmp;
	node_t *iter;

	if (data != NULL) {
		for_each_node(iter, lst) {
			if (iter->data == data)
				break;
		}
		/* not found or the node will be deleted is list_head */
		if (iter == lst->head || iter->next == lst->head)
			return NULL;
	} else {
		iter = lst->head;
	}
	
	tmp = iter->next->data;
	free(iter->next);
	
	iter->next = iter->next->next;
	iter->next->prev = iter;
	lst->node_cnt--;
	
	return tmp;
}
void *list_del_after(list_t *lst, void *data)
{
	void *tmp;
	node_t *iter;
	
	if (data != NULL) {
		for_each_node(iter, lst) {
			if (iter->data == data)
				break;
		}
	/* not found or the node will be deleted is list_head */
		if(iter->prev == lst->head || iter == lst->head)
			return NULL;
	} else {
		iter = lst->head;
	}
	
	tmp = iter->prev->data;
	free(iter->prev);
	
	iter->prev = iter->prev->prev;
	iter->prev->next = iter;

	lst->node_cnt--;
	return tmp;
}
int list_del_node(list_t *lst, void *data)
{
	node_t *iter;
	
	if (data == NULL || lst->node_cnt == 0)
		return -1;
	
	for_each_node(iter, lst) {
		if (iter->data == data) {
			iter->prev->next = iter->next;
			iter->next->prev = iter->prev;
			
			lst->node_cnt--;
			free(iter);
			return 0;
		}
	}
	return -1;
}

int list_insert_next(list_t *lst, void *tag_data, void *inst_data)
{	
	if (inst_data == NULL)
		return -1;

	node_t *node1;
	if (tag_data != NULL) {
		for_each_node(node1, lst) {
			if (node1->data == tag_data)
				break;
		}
		/* not found */
		if (node1 == lst->head)
			return -1;
		
	} else {
		node1 = lst->head;	
	}
	
	node_t *node2 = (node_t *)malloc(sizeof(node_t));
	node2->data = inst_data;
	
	node2->prev = node1;
	node2->next = node1->next;
	node2->prev->next = node2;
	node2->next->prev = node2;
	
	lst->node_cnt++;
	return 0;
} /* node2 -> node1 */
int list_insert_after(list_t *lst, void *tag_data, void *inst_data)
{
	if (inst_data == NULL)
		return -1;

	node_t *node1;
	if (tag_data != NULL) {
		for_each_node(node1, lst) {
			if (node1->data == tag_data)
				break;
		}
		/* not found */
		if (node1 == lst->head)
			return -1;
		
	} else {
		node1 = lst->head;	
	}


	node_t *node2 = (node_t *)malloc(sizeof(node_t));
	node2->data = inst_data;
	
	node2->prev = node1->prev;
	node2->next = node1;
	node2->prev->next = node2;
	node2->next->prev = node2;
	
	lst->node_cnt++;
	return 0;
}

int list_insert_tail(list_t *lst, void *data)
{
	list_insert_after(lst, NULL, data);
	return 0;
}
int list_insert_head(list_t *lst, void *data)
{
	list_insert_next(lst, NULL, data);
	return 0;
}
void *list_del_head(list_t *lst)
{
	return list_del_next(lst, NULL);
}
void *list_del_tail(list_t *lst)
{
	return list_del_after(lst, NULL);
}
/* we sort data from max to min */
int list_sort_insert(list_t *lst, void *data, int (*cmp_fn)(void *d1, void *d2))
{
	node_t *iter;
	
	if (data == NULL)
		return -1;

	if (lst->node_cnt == 0)
		list_insert_tail(lst, data);
	
	else if (!cmp_fn(data, (lst->head->prev)->data))
		list_insert_tail(lst, data);
	else {
		for_each_node(iter, lst) {
			if (cmp_fn(data, iter->data)) {
				list_insert_after(lst, iter->data, data);
				break;
			}
		}
	}
	return 0;
}
