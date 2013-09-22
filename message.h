#ifndef __MESSAGE_H
#define __MESSAGE_H

enum msg_type {
	MSG_LS,
	MSG_LS_ACK,
	MSG_CD,
	MSG_CD_ACK,
	MSG_GET,
	MSG_FILE,
	MSG_FILE_ERR,
};

struct message {
	int type;
	char data[1];
};

#define HEAD_SIZE 4
#define DATA_SIZE 4096
#define BUF_SIZE (HEAD_SIZE + DATA_SIZE)

#define PORT 3333
#endif
