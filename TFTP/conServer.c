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
#include <errno.h>
#include <fcntl.h>
#include "packet.h"

#define MAXLINE 2048
#define CONNECTIONS 3
#define DATA_LEN 512

int main(int argc, char **argv[]) {


	// Get port to run on

	if(argc != 2) {
		fprintf(stderr,"The usage is: ./server <port number>\n");
		exit(1);
	}

	int port;
	port = atoi((const char *)argv[1]);

	// Create the client and server structs
	struct sockaddr_in server,client;
	struct sockaddr_in *cliPtr; cliPtr = &client;
	socklen_t clilen; clilen = sizeof(*cliPtr);

	bzero(&client,sizeof(client));
	bzero(&server,sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);

	// Create the message buffer and declare the socket fd
	char mesg[MAXLINE];
	int sockfd, maxfd;

	// Create the readset 
	fd_set readset;

	// Create a socket

	sockfd = socket(AF_INET,SOCK_DGRAM,0);
        if(sockfd == -1) {
                perror("socket");
                exit(1);
        }

        // bind the socket to the port
        if(bind(sockfd, (struct sockaddr *)&server,sizeof(server)) == -1) {
                perror("bind");
                exit(1);
        }

	// Set the sockfd to the current maxfd
	maxfd = sockfd;

	// Declare and initalize array of connection structs
	// These keep the states of each connection
	Conn *connList[CONNECTIONS]; // 10 concurrent connections
	
	// Declare temp struct
	struct sockaddr_in tempStruct;


	int i;
	for(i = 0; i < CONNECTIONS; i++) {
		if(i == 0) { // Special case for initial fd
			// Don't mess with this guy!!!
			connList[i] = (Conn *)malloc(sizeof(Conn));
		
			Conn *temp;
			temp = connList[i];
			
			temp->blockNum = 0;
			temp->clifd = sockfd;
			temp->clientInfo = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
			temp->serverInfo = &server;
			temp->lastPacket = 0;
			temp->open = 0;
			temp->offset = 0;
			memset(temp->oldData,'\0',512);
		} else {
			connList[i] = (Conn *)malloc(sizeof(Conn));
		
			Conn *temp;
			temp = connList[i];
			
			temp->blockNum = 0;
			temp->clientInfo = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
			temp->serverInfo = &server;
			temp->lastPacket = 0;
			temp->open = 0;
			temp->offset = 0;
			memset(temp->oldData,'\0',512);
			temp->clifd = socket(AF_INET,SOCK_DGRAM,0); // marks a node that was just created.
			bind(temp->clifd,(struct sockaddr *)&server,sizeof(server));
			
			if(temp->clifd > maxfd)
				maxfd = temp->clifd;

		}
	}






















































	



	for(;;) {
	
		sleep(3);	
		// Initialize the readset
		FD_ZERO(&readset);
		printf("Initialized readset\n");

		// Set the sockfd to the current maxfd
		maxfd = sockfd;


		// Add all sockets to the readset
		int i;
		for(i = 0; i < CONNECTIONS; i++) {
			FD_SET(connList[i]->clifd,&readset);
			if((connList[i]->clifd) > maxfd)
				maxfd = connList[i]->clifd;
		}
	
		maxfd += 1; // Need to have one more than the maximum


		// Clear the memory of the client structure
		bzero(&client,sizeof(client));

		printf("Calling select to see which sockets are ready\n");
		int numFds; // The number of available connections	
		// Get back all the connections that are ready
		if((numFds = select(maxfd+1,&readset,NULL,NULL,NULL)) < 0) {
			if(errno == EINTR)
				continue;
			else
				perror("select");
		}


		if(numFds > 0) {
			printf("Returned from select. %u descriptors are available.\n",numFds);
			for(i = 0; i < CONNECTIONS; i++) {
				// Check to see if each fd is ready. If it is, then do communication
				int currFd;
				currFd = connList[i]->clifd;
				
				if(FD_ISSET(currFd, &readset)) {

					printf("Socket %u is being serviced\n",currFd);
					int len; len = sizeof(*cliPtr);
					int bytesRead;
					memset(mesg,'\0',MAXLINE);

					// Create temporary client to retrieve information
					struct sockaddr_in tempClient;
					socklen_t tempCliSize; tempCliSize = sizeof(tempClient);


					// Get the client request
					bytesRead = recvfrom(currFd,mesg,MAXLINE,0,(struct sockaddr *)&tempClient,&tempCliSize);
					
					printf("Read %u bytes\n",bytesRead);	

					Conn *connected;
					connected = connList[i];


					Request newRequest;
					Request *reqRef; reqRef = &newRequest;
					memset(reqRef->buffer,'\0',MAXLINE);
					memcpy(reqRef->buffer,mesg,bytesRead);

					organizeRequest(reqRef);	

					// File descriptor for requested file
					int fd;

					static unsigned char oldData[DATA_LEN];
					unsigned char data[DATA_LEN];


					// Print the request to the screen
					printRequest(reqRef,connected->clientInfo); 			

					switch(reqRef->opcode) {
						case 1: // RRQ
							
							// Save client information in the struct
							connected->blockNum = 1;
							connected->open = 1;
							memset(connected->filename,'\0',MAXLINE);
							strcpy(connected->filename,reqRef->filename);
							memcpy(connected->clientInfo,&tempClient,tempCliSize);

							// Check if file exists
							if(checkForFile(reqRef->filename) == 0) {
								sendErrorPacket(currFd,tempClient,tempCliSize);
								int received;
								received = recvfrom(currFd,mesg,MAXLINE,0,(struct sockaddr *)&tempClient,&tempCliSize);
								connected->blockNum = 1;
								memset(mesg,'\0',MAXLINE);
								memset(oldData,'\0',DATA_LEN);
								memset(data,'\0',DATA_LEN);
								bzero(reqRef,sizeof(Request));
								continue;
							} else
								fd = open(reqRef->filename,O_RDONLY);		
							connected->offset = 0;
							int k;	
							k = read(fd,data,512);
							strncpy(connected->oldData,data,k);
							connected->offset += k;

							// Construct packet and send
							sendDataPacket(connected->clifd,connected->blockNum,data,connected->clientInfo,sizeof(*(connected->clientInfo)),&server);
							if(k < 512)
								connected->lastPacket = 1;

							memset(data,'\0',DATA_LEN);
							break;
						case 2:
							printf("Do WRQ stuff. Most likely nothing\n");
							break;
						case 3:
							printf("DATA packet. Will not get DATA packet from client\n");
							break;

						case 4:
							printf("ACK packet. Send the next packet\n");
							
							if(connected->lastPacket == 1) {
								// Done with the transaction
								connected->blockNum = 0;
								connected->lastPacket = 0;
								connected->offset = 0;
								memset(connected->filename,'\0',MAXLINE);
								memset(connected->oldData,'\0',512);
								bzero(connected->clientInfo,sizeof(*(connected->clientInfo)));
								break;
							} else {

								k = read(fd,data,512);
								strncpy(oldData,data,k);
								connected->blockNum += 1;
								connected->offset += k;	
								// Construct packet and send
								sendDataPacket(connected->clifd,connected->blockNum,data,connected->clientInfo,sizeof(*(connected->clientInfo)),&server);
								if(k < 512)
									connected->lastPacket = 1;
	
								memset(data,'\0',DATA_LEN);
								break;
							}

						case 5:
							printf("Error packet. Retransmit previous packet\n");
							// Construct packet and send
							connected->blockNum -= 1;
							sendDataPacket(connected->clifd,connected->blockNum,data,connected->clientInfo,sizeof(*(connected->clientInfo)),&server);
							break;
						default:
							fprintf(stderr,"Error: packet not recognized\n");
							exit(1);
							break;
					}

					memset(mesg,'\0',MAXLINE);
					// Got request from client, now do 1 packet's worth of work on it.
				} else
					continue;

			}
		}			
	}	
}
