#ifndef __SER_MSG_H
#define __SER_MSG_H

struct client {
       int fd;
       char *msg_buf;
       unsigned int msg_size;
};
void ser_msg_handler(void *);

#endif 
