#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "transmit.h"

#define PIECE_SIZE (PACK_SIZE + sizeof(unsigned int))

int percent;
static unsigned int __readn(int fd, void *data, unsigned int size)
{
	unsigned int remain = size;
	unsigned int nbytes;

	char *buf = (char *)data;
	
	while ((nbytes = read(fd, buf, remain))) {		
		remain -= nbytes;
		if (remain > 0)
			buf += nbytes;
		else
			break;

	}
	return (size - remain);
}

static unsigned int __writen(int fd, void *data, unsigned int size)
{
	unsigned int remain = size;
	unsigned int nbytes;

	char *buf = (char *)data;
	
	while ((nbytes = write(fd, buf, remain))) {
		remain -= nbytes;
		if (remain > 0)
			buf += nbytes;
		else
			break;
	}
	return (size - remain);
}

void send_file(int sockfd, int fd)
{
	int pack_cnt;
	struct stat st;
	unsigned int fsize;
	struct file_piece piece;
	
	unsigned int rnbytes;
	unsigned long long cur_sum = 0;

	fstat(fd, &st);
	fsize = st.st_size;

	if (fsize % PACK_SIZE)
		pack_cnt = fsize / PACK_SIZE + 1;
	else
		pack_cnt = fsize / PACK_SIZE;
	
	printf ("%d %d\n", fsize, pack_cnt);

	int i;
	for (i = 0; i < pack_cnt; i++) {
		piece.pi_idx = i;
		rnbytes = __readn(fd, &piece.pi_data, PACK_SIZE);
		__writen(sockfd, &piece, rnbytes + sizeof(piece.pi_idx));

		cur_sum += rnbytes;
		percent = (cur_sum*100) / fsize;
	}
}
void recv_file(int sockfd, void *data)
{
	int fd;
	int pack_cnt, i;
	struct file_piece piece;
	unsigned int rnbytes;
	
	unsigned long long cur_sum = 0;
	struct file_inf *finf = (struct file_inf *)data;
	
	chdir(getenv("HOME"));
		
	fd = open(finf->fi_name, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	
	if (finf->fi_size % PACK_SIZE)
		pack_cnt = finf->fi_size / PACK_SIZE + 1;
	else
		pack_cnt = finf->fi_size / PACK_SIZE;
	
	for (i = 0; i < pack_cnt; i++) {
		if (i != pack_cnt - 1)
			rnbytes = __readn(sockfd, &piece, PIECE_SIZE);
		else  /* the last packet, its size may be less than PIECE_SIZE */
			rnbytes = __readn(sockfd, &piece, sizeof(unsigned int) + finf->fi_size - cur_sum);
		
		lseek(fd, piece.pi_idx * PACK_SIZE, SEEK_SET);
		__writen(fd, &piece.pi_data, rnbytes - sizeof(piece.pi_idx));
		
		cur_sum += rnbytes - sizeof(piece.pi_idx);
		percent = (cur_sum * 100) / finf->fi_size;
	}
}

















