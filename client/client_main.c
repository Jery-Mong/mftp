#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <poll.h>

#include "client_cmd.h"
#include "client_msg.h"
#include "message.h"

int global_fd;

int main(int argc, char *argv[])
{
	int len;
	char command[128];
	char **cmdl;
	int works;

	global_fd = 0;
	if (argc == 2)
		if (connect_to_server(argv[1]))
			return -1;
		
	char *rev_buf = (char *)malloc(BUF_SIZE);
	

	struct pollfd pfd[2];
	
	pfd[0].fd = fileno(stdin);
	pfd[0].events = POLLIN;
	pfd[1].events = POLLIN;

	for (;;) {		
		printf("M >> ");
		fflush(stdout);
		
		if (global_fd != 0) {
			pfd[1].fd = global_fd;
			works = poll(pfd, 2, -1);
		} else
			works = poll(pfd, 1, -1);

		if (pfd[0].revents == POLLIN) {
			fgets(command, 128, stdin);
			cmdl = cmdstr_to_list(command);
			if (cmdl != NULL)
				cmd_handler(cmdl);
			
			if (--works == 0)
				continue;
		}
		
		if (pfd[1].revents == POLLIN) {
			memset(rev_buf, 0, BUF_SIZE);
			len = read(global_fd, rev_buf, BUF_SIZE);

			if (len > 0) {
				client_msg_handler(rev_buf, len);
			} else {
				printf ("\nError: Server Disconnected\n");
				global_fd = 0;
			}
		}
	}

	return 0;
}
