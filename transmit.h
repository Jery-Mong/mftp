#ifndef __TRANSMIT_H
#define __TRANSMIT_H

#define PACK_SIZE 512

struct file_piece {
	int pi_idx;
	char pi_data[PACK_SIZE];
};

struct file_inf {
	char fi_name[32];
	union {
		unsigned int finf_size;
		int finf_errno;
	}inf;
};
#define fi_size inf.finf_size
#define fi_errno inf.finf_errno

extern void send_file(int, int);
extern void recv_file(int, void *);

#endif
