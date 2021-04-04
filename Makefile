CC = gcc
CFLAGS = -Wall -Wextra

NAME = traceroute
SRC = main.c icmp_receiver.c icmp_sender.c icmp_checksum.c stuff.c 
DEPS = icmp_receiver.h icmp_sender.h icmp_checksum.h stuff.h
OBJS = main.o icmp_receiver.o icmp_sender.o icmp_checksum.o stuff.o

YOU :$(SRC) $(NAME)

$(NAME):$(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME) $(LFLAGS)

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS)

distclean:
	rm -f $(OBJS) $(NAME)