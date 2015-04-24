#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <inttypes.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdint.h>
#include "packet.h"

#define PORT 5000
#define MAXLINE 2048
#define PACKETS 10


int main(int argc, char **argv) {

	int sockfd;
	struct sockaddr_in server, client;
	struct sockaddr_in *cliPtr; cliPtr = &client;

	
	// create the socket
	sockfd = socket(AF_INET,SOCK_DGRAM,0);
	if(sockfd == -1) {
		perror("socket");
		exit(1);
	}

	// initialize server struct
	bzero(&server,sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(PORT);

	// bind the socket to the port
	if(bind(sockfd, (struct sockaddr *)&server,sizeof(server)) == -1) {
		perror("bind");
		exit(1);
	}


	int i, received;
	printf("Starting to receive packets\n");	

	i = newRead(sockfd,&client,&server);

	return 0;
}
