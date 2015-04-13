CC=gcc
CFLAGS=-I.
DEPS = packet.h
OBJ = packet.o server.o func.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

server: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -rf *.o server


#all: server.c client.c packet.h packet.c func.c
#	clear
#	gcc -o server server.c packet.h packet.c func.c -g
#	gcc -o client client.c packet.h packet.c func.c -g

#clean:
#	rm -rf server client
