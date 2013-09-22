#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "ser_local.h"
#include "message.h"
#include "transmit.h"

unsigned int __popen(char *cmd, char *buf, int buf_size)
{

	int fd[2];
	pipe(fd);
	
	if (fork() == 0) {
		close(fd[0]);
		dup2(fd[1], 1);
		close(fd[1]);
		execl("/bin/sh", "sh", "-c", cmd, NULL);
	} else {
		close(fd[1]);
		wait(NULL);

	}
	return read(fd[0], buf, buf_size);
}	

void msg_shell_cmd(struct client *clnt)
{
	struct message *msg = (struct message *)(clnt->msg_buf);

	clnt->msg_size = __popen(msg->data, msg->data, DATA_SIZE);

	clnt->msg_size += sizeof(msg->type);
	msg->type++;
	
	send(clnt->fd, msg, clnt->msg_size, 0);
}
void msg_chdir(struct client *clnt)
{
	struct message *msg = (struct message *)(clnt->msg_buf);
	
	chdir(msg->data + 3);
	
	clnt->msg_size = __popen("pwd", msg->data, DATA_SIZE);
	
	clnt->msg_size += sizeof(msg->type);
	msg->type++;	
	send(clnt->fd, msg, clnt->msg_size, 0);
}
void msg_send_file(struct client *clnt)
{
	struct message *msg = (struct message *)(clnt->msg_buf);
	struct file_inf *finf = (struct file_inf *)(msg->data);

	strcpy(finf->fi_name, msg->data);
	
	int fd = open(msg->data, O_RDONLY);

	if (fd < 0) {
		msg->type = MSG_FILE_ERR;
		finf->fi_errno = errno;
		send(clnt->fd, msg, sizeof(msg->type) + sizeof(struct file_inf), 0);
	} else {
		msg->type = MSG_FILE;
		struct stat st;
		fstat(fd, &st);
		
		finf->fi_size = st.st_size;
		
		send(clnt->fd, msg, sizeof(msg->type) + sizeof(struct file_inf), 0);

		usleep(100000);	/* 100ms */
		send_file(clnt->fd, fd);
	}
}
void *ser_msg_handler(void *data)
{
	struct client *clnt = (struct client *)data;
	struct message *msg = (struct message *)(clnt->msg_buf);
	
	switch (msg->type) {
	case MSG_LS:
		msg_shell_cmd(clnt);
		break;
	case MSG_CD:
		msg_chdir(clnt);
		break;
	case MSG_GET:
		msg_send_file(clnt);
		break;
	}
	return NULL;
}
