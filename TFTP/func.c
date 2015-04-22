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
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "packet.h"

#define PORT 5000
#define MAXLINE 2048
#define PACKETS 10
#define DATA_PACK_LEN 516
#define DATA_LEN 512

State packState;
uint16_t blockNum;


void printRequest(Request *request,struct sockaddr_in *cliPtr) {

        char summary[MAXLINE];
	memset(summary,'\0',MAXLINE);

        struct sockaddr_in clientInfo;
        clientInfo = *cliPtr;

        if(request->opcode == 1)
                sprintf(summary,"%s %s %s from %s:%u\n","RRQ",request->filename, request->mode,inet_ntoa(clientInfo.sin_addr),ntohs(clientInfo.sin_port));
        else if(request->opcode == 2)
                sprintf(summary,"%s %s %s from %s:%u\n","WRQ",request->filename, request->mode,inet_ntoa(clientInfo.sin_addr),ntohs(clientInfo.sin_port));


        printf("%s",summary);

}

// Return 1 on error
// Return 0 on success
int organizeRequest(Request *newRequest) {

        /*
	* 1) Get first two bytes, convert them from network to host byte order, and store
        * 2) Advance pointer by two bytes
        * 3) Read characters into a buffer until you hit the null character. This is the filename
        * 4) Advance pointer past the null character
        * 5) If the first character is "n", read 8 bytes.
        *       if "o", read 5 bytes
        *       if "m", read 4 bytes
        */


        char reqContent[MAXLINE];
        memcpy(reqContent,newRequest->buffer,MAXLINE);
        char *it; it = reqContent;

        // Step 1
        memcpy(&(newRequest->opcode),reqContent,2);
        newRequest->opcode = ntohs(newRequest->opcode);


	uint16_t tempOpcode;
	tempOpcode = newRequest->opcode;

	
	if(tempOpcode == 3) {
		fprintf(stderr,"Packet is %s. This request will be handled in the second part of the assignment\n","DATA");
		return 1;
	} else if(tempOpcode == 4) {
		//printf("Packet is %s. This request will be handled in the second part of the assignment\n","ACK");
		return 0;
	} else if(tempOpcode == 5) {
		fprintf(stderr,"Packet is %s. This request will be handled in the second part of the assignment\n","ERROR");
		return 1;
	}




        // Step 2
        it += 2; // advance two bytes

        // Step 3

        int fileLength; fileLength = strlen(it);
        strncpy(newRequest->filename,it,fileLength);
        
        // Step 4
        it =  it + (fileLength + 1);
       
	// Step 5

        if((*it == 'n') && (strncmp(it,"netascii",8) == 0)) {
		strncpy(newRequest->mode,it,8);
        	newRequest->mode[8] = '\0'; // properly terminate the string
        } else if((*it == 'o') && (strncmp(it,"octet",5) == 0)) {
        	strncpy(newRequest->mode,it,5);
        	newRequest->mode[5] = '\0'; // properly terminate the string
        } else if((*it == 'm') && (strncmp(it,"mail",4) == 0)) {
        	strncpy(newRequest->mode,it,4);
        	newRequest->mode[4] = '\0'; // properly terminate the string
        } else {
        	fprintf(stderr,"Mode not correctly defined. Create error packet\n");
		return 1;
	}

	return 0;

}

int sendErrorPacket(int sockfd,struct sockaddr_in client, socklen_t clilen) {
	uint16_t opcode, errorCode;
	opcode = htons(5);
	errorCode = htons(1);
	char *fileNotFound;
	fileNotFound = "File not found";

	// Allocate space for packet and initialize it
	int packetLen; packetLen = 2 + 2 + strlen(fileNotFound) + 1;	
	char errorPacket[packetLen];
	memset(errorPacket,'\0',packetLen);

	char *memPtr; memPtr = errorPacket;

	// Copy in the opcode
	memcpy(memPtr,&opcode,2);
	memPtr += 2;

	// Copy in the error code
	memcpy(memPtr,&errorCode,2);
	memPtr += 2;

	// Copy in the File Not Found error
	memcpy(memPtr,fileNotFound,strlen(fileNotFound));
	memPtr += strlen(fileNotFound);
	*memPtr = '\0';	

	// Send the packet	
	int error;		
	error = sendto(sockfd,errorPacket,packetLen,0,(struct sockaddr *)&client,clilen);

	if(error == 1) {
		fprintf(stderr,"Error creating packet\n");
		return 1;
	} else
		return 0;
}

/*
This function checks to see whether a file exists in the current directory.
Returns 1 if the file is present, and 0 if the file is not present
*/
int checkForFile(char *filename) {

	// Open current directory
	DIR *dp = opendir(".");

	// Check each entry for the file
	struct dirent *entry;
	while((entry = readdir(dp)) != NULL) {
		if(strcmp(entry->d_name,filename) == 0) {
			closedir(dp);
			return 1;
		}
	}

	closedir(dp);	
	return 0;

}



/*
This function constructs a data packet and sends it to the client
*/


int sendDataPacket(int sockfd, int replyfd, int blockNum, char *data, struct sockaddr_in *cliPtr, socklen_t clilen, struct sockaddr_in *servPtr) {

	int packetSize;
	packetSize = 4 + strlen(data);
	char packet[packetSize];
	memset(packet,'\0',packetSize);
	char *iter; iter = packet;

	// Copy opcode in
	uint16_t opcode = htons(3);
	memcpy(iter,&opcode,2);
	iter += 2;

	// Copy block number in
	uint16_t netBlockNum; 
	memset(&netBlockNum,'\0',2);
	netBlockNum = htons(blockNum);
	memcpy(iter,&netBlockNum,2);
	iter += 2;

	// Copy data in 
	memcpy(iter,data,strlen(data));

	// send the message

	// declare a new socket

//	int replyfd;
//	replyfd = socket(AF_INET,SOCK_DGRAM,0);

	//bind(replyfd,(struct sockaddr *)servPtr, sizeof(*servPtr));
	int error;
	error = sendto(replyfd,packet,packetSize,0,(struct sockaddr *)cliPtr,clilen);
	//error = sendto(sockfd,packet,packetSize,0,(struct sockaddr *)cliPtr,clilen);



}

int newRead(int sockfd, struct sockaddr_in *cliPtr, struct sockaddr_in *servPtr) {

	// Initialize data and message structures	
	unsigned char data[DATA_LEN];
	unsigned char mesg[MAXLINE];
	unsigned char oldData[DATA_LEN];
	memset(data,'\0',DATA_LEN);
	memset(mesg,'\0',MAXLINE);
	memset(oldData,'\0',DATA_LEN);

	// Initialize client structure
	socklen_t clilen; 
	// Initialize references to a Request
	int error;
	Request newRequest;
	Request *reqRef;
	reqRef = &newRequest;
	bzero(reqRef,sizeof(Request));

	int fd;
	int blockNum; blockNum = 1;
	int k; 

	// Loop for each download

	for(;;) {
		// Receive the first message
		printf("Waiting to initiate transfer...\n");
		memset(&clilen,'\0',sizeof(socklen_t));
		clilen = sizeof(*cliPtr);

		int received;
		received = recvfrom(sockfd,mesg,MAXLINE,0,(struct sockaddr *)cliPtr,&clilen);
		memcpy(newRequest.buffer,mesg,MAXLINE);
		error = organizeRequest(reqRef);

		// Need to check if file exists
		if(!checkForFile(reqRef->filename)) {
			sendErrorPacket(sockfd,*cliPtr,clilen);
			received = recvfrom(sockfd,mesg,MAXLINE,0,(struct sockaddr *)cliPtr,&clilen);
			blockNum = 1;
			memset(mesg,'\0',MAXLINE);
			memset(oldData,'\0',DATA_LEN);
			memset(data,'\0',DATA_LEN);
			bzero(reqRef,sizeof(Request));
			continue;
		} else
			fd = open(reqRef->filename,O_RDONLY);

		// Loop for each request within file
		k = 512;
		int replyfd;
		while(k == 512) {
		
			// Print the request	
			printRequest(reqRef,cliPtr);
			replyfd = socket(AF_INET,SOCK_DGRAM,0);	
			switch(reqRef->opcode) {
				case 1: // RRQ
					//printf("Do RRQ stuff\n");
					k = read(fd,data,512);
					strncpy(oldData,data,k);
					oldData[k] = '\0';
					// Construct packet and send
					sendDataPacket(sockfd,replyfd,blockNum,data,cliPtr,clilen,servPtr);
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
					k = read(fd,data,512);
					strncpy(oldData,data,k);
					oldData[k] = '\0';
					// Construct packet and send
					sendDataPacket(sockfd,replyfd,blockNum,data,cliPtr,clilen,servPtr);
					memset(data,'\0',DATA_LEN);
					break;

				case 5: 
					printf("Error packet. Retransmit previous packet\n");
					// Construct packet and send
					blockNum -= 1;
					sendDataPacket(sockfd,replyfd,blockNum,oldData,cliPtr,clilen,servPtr);
					break;
				default:
					fprintf(stderr,"Error: packet not recognized\n");
					exit(1);
			}
			memset(mesg,'\0',MAXLINE);	
//			received = recvfrom(sockfd,mesg,MAXLINE,0,(struct sockaddr *)cliPtr,&clilen);

			printf("BlockNum: %u. Read %u bytes\n",blockNum,k);
			blockNum += 1;
	
			
			printf("Waiting for next request...\n");	
			memset(mesg,'\0',MAXLINE);
			//received = recvfrom(sockfd,mesg,MAXLINE,0,(struct sockaddr *)cliPtr,&clilen);
			received = recvfrom(replyfd,mesg,MAXLINE,0,(struct sockaddr *)cliPtr,&clilen);

			memcpy(newRequest.buffer,mesg,MAXLINE);
			error = organizeRequest(reqRef);

		}
		close(fd);
		blockNum = 1;
		memset(mesg,'\0',MAXLINE);
		memset(oldData,'\0',DATA_LEN);
		memset(data,'\0',DATA_LEN);
		bzero(reqRef,sizeof(Request));

	}
}



















