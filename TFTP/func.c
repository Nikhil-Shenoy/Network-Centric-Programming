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
#include "packet.h"

#define PORT 5000
#define MAXLINE 2048
#define PACKETS 10

void printRequest(Request *request,struct sockaddr_in *cliPtr) {

        char summary[MAXLINE];

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
int parseRequest(Request *newRequest) {

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
		fprintf(stderr,"Packet is %s. This request will be handled in the second part of the assignment\n","ACK");
		return 1;
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

	int packetLen; packetLen = 2 + 2 + strlen(fileNotFound) + 1;	
	char errorPacket[packetLen];
	memset(errorPacket,'\0',packetLen);

	char *memPtr; memPtr = errorPacket;
	
	memcpy(memPtr,&opcode,2);
	memPtr += 2;

	memcpy(memPtr,&errorCode,2);
	memPtr += 2;

	memcpy(memPtr,fileNotFound,strlen(fileNotFound));
	memPtr += strlen(fileNotFound);
	*memPtr = '\0';	
	
	int error;		
	error = sendto(sockfd,errorPacket,packetLen,0,(struct sockaddr *)&client,clilen);

	if(error == 1) {
		fprintf(stderr,"Error creating packet\n");
		return 1;
	} else
		return 0;
}

















