clientfiles=client_main.c client_msg.c client_cmd.c transmit.c 

CFLAGS=-Wall -I../ -pthread

client:$(clientfiles:.c=.o)
	gcc $(CFLAGS) $^ -o $@
%.o:%.c
	gcc $(CFLAGS) -c $<

clean:
	-rm *.o
cleanall:
	-rm  client *.o

.PHONY:clean cleanall 
