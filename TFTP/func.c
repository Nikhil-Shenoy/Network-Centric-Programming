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

/*
int sendDataPacket(int sockfd, struct sockaddr_in client, socklen_t clilen, char *filename, char *mode) {

	// Update state struct
	strcpy(packState.Filename,filename);
	strcpy(packState.Mode,mode);
	packState.BlockNum = htons(1);
	packState.Offset = 0;
	packState.Opcode = htons(3);
	
	// Create space for data and previous data
	unsigned char data[DATA_LEN];
	memset(data,'\0',DATA_LEN);
	
	// Create space for offset and bytesRead
	//int offset; offset = 0;
	int bytesRead; bytesRead = 0;

	// Gather data from file
	FILE *fp = fopen(filename,"r");
	if(fp == NULL) {
		perror("Error opening file");
		return 1;
	}

	char c; int i; i = 0;
	c = fgetc(fp);
	while(c != EOF)  {
		data[i] = c;
		i++;		
		c = fgetc(fp);
		if((i == 512) || (c == EOF))
			break;	
	}
	fclose(fp);
	bytesRead = i;

	strcpy(packState.Data,data);
	packState.Data[strlen(data) + 1] = '\0';
	packState.BytesRead = bytesRead;


	// Create packet
	int packetsize; packetsize = 4 + bytesRead;
	char packet[packetsize];
	memset(packet,'\0',packetsize);
	char *iterator; iterator = packet;
	
	// Copy in the opcode
	memcpy(iterator,&packState.Opcode,2);
	iterator += 2;
	
	// Copy in the block number
	memcpy(iterator,&packState.BlockNum,2);
	iterator += 2;

	// Copy in the data
	memcpy(iterator,data,strlen(data));
	iterator += strlen(data);
	*iterator = '\0';

	// Send the packet
	int error;		
	error = sendto(sockfd,packet,packetsize,0,(struct sockaddr *)&client,clilen);


	if(error == 1) {
		fprintf(stderr,"Error sending DATA packet\n");
		return 1;
	} else
		return 0;

}
*/


int sendDataPacket(int sockfd, int blockNum, char *data, struct sockaddr_in *cliPtr, socklen_t clilen) {

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
	int error;
	error = sendto(sockfd,packet,packetSize,0,(struct sockaddr *)cliPtr,clilen);


}

int newRead(int sockfd, struct sockaddr_in *cliPtr) {

	// Initialize data and message structures	
	unsigned char data[DATA_LEN];
	unsigned char mesg[MAXLINE];
	memset(data,'\0',DATA_LEN);
	memset(mesg,'\0',MAXLINE);

	// Initialize client structure
	socklen_t clilen; memset(&clilen,'\0',sizeof(socklen_t));
	clilen = sizeof(*cliPtr);

	// Initialize references to a Request
	int error;
	Request newRequest;
	Request *reqRef;
	reqRef = &newRequest;

	int fd;
	int blockNum; blockNum = 1;
	int k; k = 512;

	// Loop for each download

	for(;;) {
		// Receive the first message
		int received;
		received = recvfrom(sockfd,mesg,MAXLINE,0,(struct sockaddr *)cliPtr,&clilen);
		memcpy(newRequest.buffer,mesg,MAXLINE);
		error = organizeRequest(reqRef);
	
		if(reqRef->opcode != 1) {
			fprintf(stderr,"Initial request is not RRQ\n");
		} else {

			fd = open(reqRef->filename,O_RDONLY);
	
			// Loop for each request within file
			while(k == 512) {
			
				// Print the request	
				printRequest(reqRef,cliPtr);
		
				switch(reqRef->opcode) {
					case 1: // RRQ
						//printf("Do RRQ stuff\n");
						if(checkForFile(reqRef->filename)) {
							k = read(fd,data,512);
							// Construct packet and send
							sendDataPacket(sockfd,blockNum,data,cliPtr,clilen);
							break;
						} else {
							fprintf(stderr,"Error: File not found. Send error packet\n");
							break;
						}
					case 2:
						printf("Do WRQ stuff. Most likely nothing\n");
						break;
					case 3:
						printf("DATA packet. Will not get DATA packet from client\n");
						break;
					case 4:
						printf("ACK packet. Send the next packet\n");
						k = read(fd,data,512);
						// Construct packet and send
		
					case 5: 
						printf("Error packet. Retransmit previous packet\n");
						// Construct packet and send
					default:
						fprintf(stderr,"Error: packet not recognized\n");
						exit(1);
				}
		
				printf("BlockNum: %u. Read %u bytes\n",blockNum,k);
				blockNum++;
			
				received = recvfrom(sockfd,mesg,MAXLINE,0,(struct sockaddr *)cliPtr,&clilen);
				memcpy(newRequest.buffer,mesg,MAXLINE);
				error = organizeRequest(reqRef);


			}
			close(fd);
		}
	}
}



















