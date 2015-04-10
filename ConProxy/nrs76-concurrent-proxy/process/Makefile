#Sample Makefile. You can make changes to this file according to your need
# The executable must be named proxy

CC = gcc
CFLAGS = -Wall -g 
LDFLAGS = -lpthread

OBJS = proxy.o csapp.o

all: proxy

proxy: $(OBJS)
	$(CC) -o proxy $(OBJS) $(LDFLAGS)

csapp.o: csapp.c csapp.h
	$(CC) $(CFLAGS) -c csapp.c 

proxy.o: proxy.c
	$(CC) $(CFLAGS) -c proxy.c

client: client.c
	$(CC) $(CFLAGS) -o client client.c csapp.h csapp.c $(LDFLAGS)
clean:
	rm -f *~ *.o proxy client

