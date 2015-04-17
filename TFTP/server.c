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

	i = newRead(sockfd,&client);
/*
	for(;;) {
		unsigned char mesg[MAXLINE];
		memset(mesg,'\0',MAXLINE);
		socklen_t clilen; memset(&clilen,'\0',sizeof(socklen_t)); clilen = sizeof(client);
		received = recvfrom(sockfd,mesg,MAXLINE,0,(struct sockaddr *)&client,&clilen);
		if(received == -1) {
			perror("recvfrom");
		} else {

			int error;
			Request newRequest;
			Request *reqRef;
			memcpy(newRequest.buffer,mesg,MAXLINE);
			reqRef = &newRequest;
			
			error = organizeRequest(reqRef);	
			if(error == 1) {
				fprintf(stderr,"Error in organizing request\n");
				exit(1);
			}

			printRequest(reqRef,cliPtr);	

						
			// Verify if it is RRQ

			switch(reqRef->opcode) {
				case 1:  // RRQ
					printf("Do RRQ stuff\n");
					if(checkForFile(reqRef->filename)) {
						//error = sendDataPacket(sockfd,client,clilen,reqRef->filename,reqRef->mode);
						error = newRead(sockfd,client,clilen,reqRef->filename,reqRef->mode);
					}
					else
						error = sendErrorPacket(sockfd,client,clilen);
					break;
				case 2:
					printf("Do WRQ stuff. Most likely nothing\n");
					break;
				case 3:
					printf("DATA packet. Most likely won't get this from client, but should handle it anyway\n");
					break;
				case 4:
					printf("ACK packet. Send the next packet\n");
					break;
				case 5:
					printf("Error packet. Retransmit previous packet\n");
					break;
				default:
					fprintf(stderr,"Opcode not part of RFC protocol\n");
					exit(1);
			}
				
			memset(reqRef,'\0',sizeof(*reqRef));
	
			//error = sendErrorPacket(sockfd,client,clilen);	
			if(error == 1) 
				continue;
		}
	}
*/



	return 0;
}
