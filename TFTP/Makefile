all: server.c client.c packet.h packet.c
	clear
	gcc -o server server.c packet.h packet.c -g
	gcc -o client client.c packet.h packet.c -g

clean:
	rm -rf server client
