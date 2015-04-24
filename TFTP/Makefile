CC=gcc
CFLAGS=-I. -g
DEPS = packet.h func.h
OBJ = packet.o server.o func.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

server: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

con: packet.h packet.c conServer.c func.c func.h
	clear
	gcc -o con conServer.c packet.h packet.c func.c func.h $(CFLAGS)

clean:
	rm -rf *.o server test con count


#all: server.c client.c packet.h packet.c func.c
#	clear
#	gcc -o server server.c packet.h packet.c func.c -g
#	gcc -o client client.c packet.h packet.c func.c -g

#clean:
#	rm -rf server client
