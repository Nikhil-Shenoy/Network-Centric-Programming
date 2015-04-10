#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <inttypes.h>
#include <string.h>
#include <netdb.h>
#include "packet.h"

#define PORT 5000
#define MAXLINE 2048
#define PACKETS 10

void printRequest(Request *request,struct sockaddr_in *cliPtr) {

	char summary[MAXLINE];
/*
	char **pp;

	struct in_addr address;
	struct hostent *hp;

	hp = gethostbyaddr(cliPtr,sizeof(*cliPtr),AF_INET);
	pp = hp->h_addr_list;
	address.s_addr = ((struct in_addr *)*pp)->s_addr;
*/
	

	if(request->opcode == 1) 
		sprintf(summary,"%s %s %s from %u:%u\n","RRQ",request->filename, request->mode,inet_ntoa(cliPtr->sin_addr),cliPtr->sin_port);
	else if(request->opcode == 2)
		sprintf(summary,"%s %s %s from %u:%u\n","WRQ",request->filename, request->mode,inet_ntoa(cliPtr->sin_addr),cliPtr->sin_port);


	printf("%s",summary);

}



void parseRequest(Request *newRequest) {

	/*
 	* 1) Get first two bytes, convert them from network to host byte order, and store
 	* 2) Advance pointer by two bytes
 	* 3) Read characters into a buffer until you hit the null character. This is the filename
 	* 4) Advance pointer past the null character
 	* 5) If the first character is "n", read 8 bytes.
 	* 	if "o", read 5 bytes
 	* 	if "m", read 4 bytes
 	*/

	 
	char reqContent[MAXLINE];
	memcpy(reqContent,newRequest->buffer,MAXLINE);
	char *it; it = reqContent;

	// Step 1
	memcpy(&(newRequest->opcode),reqContent,2);
	newRequest->opcode = ntohs(newRequest->opcode);

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
	} else if((*it == 'o') && (strncmp(it,"octal",5) == 0)) {
		strncpy(newRequest->mode,it,5);
		newRequest->mode[6] = '\0'; // properly terminate the string
	} else if((*it == 'm') && (strncmp(it,"mail",4) == 0)) {
		strncpy(newRequest->mode,it,4);
		newRequest->mode[5] = '\0'; // properly terminate the string
	} else
		fprintf(stderr,"Mode not correctly defined. Create error packet\n");

}


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
	for(;;) {
		char mesg[MAXLINE];
		memset(mesg,'\0',MAXLINE);
		socklen_t clilen; clilen =sizeof(client);
		received = recvfrom(sockfd,mesg,MAXLINE,0,(struct sockaddr *)&client,&clilen);

		Request newRequest;
		Request *reqRef;
		memcpy(newRequest.buffer,mesg,MAXLINE);
		reqRef = &newRequest;
		
		parseRequest(reqRef);	

		printRequest(reqRef,cliPtr);	
	
			
		uint16_t opcode, errorCode;
		opcode = htons(5);
		errorCode = htons(2);
		char *fileNotFound;
		fileNotFound = "File not found";

		int packetLen; packetLen = 2 + 2 + strlen(fileNotFound) + 1;	
		char errorPacket[packetLen];
		memset(errorPacket,'\0',packetLen);
	
		memcpy(errorPacket,&opcode,2);
		memcpy(errorPacket,&errorCode,2);
		memcpy(errorPacket,fileNotFound,strlen(fileNotFound));
			
		sendto(sockfd,errorPacket,packetLen,0,(struct sockaddr *)&client,clilen);
	}




	return 0;
}
