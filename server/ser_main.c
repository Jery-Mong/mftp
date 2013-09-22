#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ser_msg.h"
#include "message.h"
#include "ser_local.h"
#include "thread_pool.h"
#include "list.h"

#define CLIENT_MAX 10

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

int main()
{
	int lfd, maxfd;
	int works;
	
	thread_pool_t tp;
	thread_pool_init(&tp, CLIENT_MAX, CLIENT_MAX);

	list_t client_lst;
	list_init(&client_lst);

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = INADDR_ANY;

	lfd = socket(AF_INET, SOCK_STREAM, 0);

	bind(lfd, (struct sockaddr *)&addr, sizeof(addr));
	listen(lfd, CLIENT_MAX);

	fd_set rset, bset;
	FD_ZERO(&bset);
	FD_SET(lfd, &bset);

	maxfd = lfd;
	for(;;) {
		rset = bset;
		works = select(maxfd + 1, &rset, NULL, NULL, NULL);
 		
		if (works > 0) {
			if (FD_ISSET(lfd, &rset)) {

				if (list_size(&client_lst) < CLIENT_MAX) {
					struct client *clnt = (struct client *)malloc(sizeof(struct client));
					clnt->msg_buf = (char *)malloc(BUF_SIZE);
					
					clnt->fd = accept(lfd, NULL, NULL);
					
					list_insert_tail(&client_lst, clnt);
					
					FD_SET(clnt->fd, &bset);
					maxfd = MAX(maxfd, clnt->fd);
				}
				if (--works == 0)
					continue;
			}

			node_t *iter;
			for_each_node(iter, &client_lst) {
				struct client *clnt = (struct client *)(iter->data);
				
				if (FD_ISSET(clnt->fd, &rset)) {
					memset(clnt->msg_buf, 0, BUF_SIZE);
					clnt->msg_size = recv(clnt->fd, clnt->msg_buf, BUF_SIZE, 0);
					
					if (clnt->msg_size > 0) {	
						thread_pool_add(&tp, ser_msg_handler, clnt);
						//ser_msg_handler(clnt);
					} else {
						list_del_node(&client_lst, clnt);
						FD_CLR(clnt->fd, &bset);
						free(clnt->msg_buf);
						free(clnt);
					}
					if (--works == 0)
						break;
				}
			}
		}
	}
}
