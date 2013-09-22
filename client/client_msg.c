#include <stdio.h>
#include <string.h>
#include <error.h>

#include "message.h"
#include "transmit.h"
extern int global_fd;

void msg_get_file(char *rev_buf)
{
	struct message *msg = (struct message *)rev_buf;
        struct file_inf *finf = (struct file_inf *)(msg->data);

        if (msg->type == MSG_FILE_ERR) {
		printf("%s: %s\n",finf->fi_name, strerror(finf->fi_errno));
		return;
	}
	printf("\nStart....\n");
	recv_file(global_fd, msg->data);
	printf("100%c \n", '%');
}

void client_msg_handler(char *rev_buf, int len)
{
	struct message *msg = (struct message *)rev_buf;
	
	switch (msg->type){
	case MSG_LS_ACK:
	case MSG_CD_ACK:
		printf("\n%s", msg->data);
		break;
	case MSG_FILE:
	case MSG_FILE_ERR:
		msg_get_file(rev_buf);
		break;
	}	
}
