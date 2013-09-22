serverfiles=ser_main.c ser_msg.c list.c thread_pool.c transmit.c

CFLAGS=-Wall -pthread

server:$(serverfiles:.c=.o)
	gcc  $(CFLAGS) $^ -o $@

%.o:%.c
	gcc $(CFLAGS) -I../ -c $<

clean:
	-rm *.o
cleanall:
	-rm  server *.o

.PHONY:clean cleanall
