#ifndef __SER_LOCAL_H
#define __SER_LOCAL_H

struct client {
	int fd;
	char *msg_buf;
	unsigned int msg_size;
};

#endif
