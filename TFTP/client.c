#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include "packet.h"

#define PORT 5000
#define MAXLINE 2048
#define PACKETS 10
#define SERVER_IP "127.0.0.1"

int main(int argc, char **argv) {

	// this contains info about the server we are going to send to
	struct sockaddr_in server;
	int sockfd, i, serverlen;
	serverlen = sizeof(server);
	char buffer[MAXLINE];

	if((sockfd=socket(AF_INET,SOCK_DGRAM,0)) == -1) {
		perror("socket");
		exit(1);
	}

	// initialize the server struct
	bzero(&server,sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	if(inet_aton(SERVER_IP,&server.sin_addr) == 0) {
		perror("inet_aton");
		exit(1);
	}

	printf("Constructing a packet...\n");
	struct request WRQ;
	constructRequest(&WRQ,1,"something.txt","netascii");

	for(i = 0; i < PACKETS; i++ ) {
		if(sendto(sockfd,&WRQ,sizeof(WRQ),0,(struct sockaddr *)&server,serverlen) == -1) {
			perror("sendto");
			exit(1);
		}
	}

	printf("Done sending!\n");


	return 0;
}
