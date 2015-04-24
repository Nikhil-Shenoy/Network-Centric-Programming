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
#include "packet.h"

#define PORT 5000
#define MAXLINE 2048
#define PACKETS 10
#define DATA_PACK_LEN 516
#define DATA_LEN 512

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

int sendDataPacket(int sockfd, struct sockaddr_in client, socklen_t clilen, char *filename,int resend) {
	
	// Create state object
	static State lastRequest;


	// Create block number
	static uint16_t blockNum; blockNum = 1;
	uint16_t convertedBlockNum; convertedBlockNum = htons(blockNum);

	// Assemble opcode
	uint16_t opcode;
	opcode = htons(3);

	// Create space for data and previous data
	unsigned char data[DATA_LEN];
	static unsigned char oldData[DATA_LEN];
	memset(data,'\0',DATA_LEN);
	
	// Create space for offset and bytesRead
	static int Offset; Offset = 0;
	int bytesRead; bytesRead = 0;

	// Handle resend condition
	if(resend == 1) {
		strcpy(data,lastRequest.Data);
		blockNum -= 1;
		bytesRead = strlen(lastRequest.Data);
	} else {	
		// Gather data from file
		FILE *fp = fopen(filename,"r");
		if(fp == NULL) {
			perror("Error opening file");
			return 1;
		}
		fseek(fp,Offset,SEEK_SET);

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
		//strcpy(oldData,data);
		bytesRead = strlen(data);
	
		strcpy(lastRequest.Data,data);	
		memcpy(&(lastRequest.blockNumber),&blockNum,2);
		lastRequest.offset = Offset;
	}


	// Create packet
	int packetsize; packetsize = 4 + bytesRead + 1;
	char packet[packetsize];
	memset(packet,'\0',packetsize);
	char *iterator; iterator = packet;
	
	// Copy in the opcode
	memcpy(iterator,&opcode,2);
	iterator += 2;
	
	// Copy in the block number
	memcpy(iterator,&convertedBlockNum,2);
	iterator += 2;

	// Copy in the data
	memcpy(iterator,data,strlen(data));
	iterator += strlen(data);
	*iterator = '\0';

	// Send the packet
	int error;		
	error = sendto(sockfd,packet,packetsize,0,(struct sockaddr *)&client,clilen);
	
	if(resend == 1) 
		blockNum += 1;
	else {
		Offset += (uint16_t)bytesRead;
		blockNum += 1;
	}


	if(error == 1) {
		fprintf(stderr,"Error sending DATA packet\n");
		return 1;
	} else
		return 0;

}












