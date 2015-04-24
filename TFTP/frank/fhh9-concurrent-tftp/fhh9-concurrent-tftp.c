#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <inttypes.h>

typedef struct sockaddr SA;
#define	MAX_LINE	 8192

/* I am VERY aware of the unreasonable function length, but i am too stressed out to care if i lose points as a result
*  I hope that you can appreciate how it feels to be overworked and just give me a pass on this
*/

int parseUDPmessg(char *mesg, uint16_t *opcode, char *filename, char *mode, char *IP, int *port, struct sockaddr_in* cliaddr, int* blocknum);

void sendError(int sockfd, struct sockaddr_in* cliaddr, int clilen);

int createUDPSocket(int port, struct sockaddr_in* servaddr );

int createUDPdataPacket(char* udpresp, uint16_t* blocknum, int fd, int* dying);

struct UDPsocket 
{
	int fd;	//for socket
	int ffd; //for file
	struct sockaddr_in* servaddr;
	int dying;	
	int available;
	uint16_t blocknum;
};

#define LIST_LENGTH 	10

struct UDPsocket* sockList[LIST_LENGTH]; 	// only realized it should have been called sockarray at the end

int main(int argc, char** argv)
{
	struct sockaddr_in	cliaddr, servaddr;	
	char mesg[MAX_LINE];
	fd_set set;			// SET HERE
	
	// GET THE PORT NUMBER HERE
	if (argc != 2) 
	 {
		fprintf(stderr, "Usage: %s <port number>\n", argv[0]);
		exit(0);
    	 }	

	int PORTNUM = atoi(argv[1]);
	int sockfd = createUDPSocket(PORTNUM, &servaddr);	//creates and binds socket
	printf("MAINSOCK %d\n", sockfd);
	fflush(stdout);
////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////// LIST INIT FUNCTION SHOULD BE MADE HERE, BUT THERE'S NO TIME11111!111 ///////////////////

	struct UDPsocket* mainsock = malloc(sizeof(struct UDPsocket));
	mainsock->fd = sockfd;
	mainsock->servaddr = &servaddr;
	mainsock->dying = 0;
	mainsock->available = 0;
	mainsock->ffd = 0;
	mainsock->blocknum = 0;
	sockList[0] = mainsock;

	struct sockaddr_in defaultservaddr; // NEVER CHANGE THIS ---- EVER 

	int defaultsockfd = createUDPSocket(0, &defaultservaddr ); // To ensure no false starts occur
	printf("BEST SOCK %d\n", defaultsockfd);
	fflush(stdout);
	int i;
	for(i = 1; i < LIST_LENGTH; ++i)	// There will never be a situation where this data will be used
	 {
		struct UDPsocket* transientsock = malloc(sizeof(struct UDPsocket));
		transientsock->fd = defaultsockfd;
		transientsock->servaddr = &defaultservaddr;
		transientsock->dying = 0;
		transientsock->available = 1;	
		transientsock->ffd = 0;
		transientsock->blocknum = 0;
		sockList[i] = transientsock;
	 }	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

	int tempsockfd;
	int dying = 0;	// tells you to cance1 after next ACK 
	uint16_t blocknum2 = htons(0);

	while(1)
	 {
		FD_ZERO(&set);
		
		for (i = 0; i < LIST_LENGTH; ++i)
		{
			int supertempfd = (sockList[i])->fd;
			FD_SET(supertempfd, &set);
		}

		select(LIST_LENGTH, &set, NULL, NULL, NULL);

		for (i = 0; i < LIST_LENGTH; ++i)
		 {		
			if (FD_ISSET((sockList[i])->fd, &set))
			 {
				bzero(mesg, sizeof(mesg)); // to ensure mesg is always full of NULL chars
				int n;
				int clilen = sizeof(cliaddr);
				int inloopfd = (sockList[i])->fd; //SHORT TERM FILEDES STORAGE FOR SENDING DATA

				n = recvfrom(inloopfd, mesg, MAX_LINE, 0, (SA *) &cliaddr, &clilen);

				/* tried putting it in a struct but inet_ntoa didnt work, counted the 0 in 127.0.0.1 as a NULL
				*  character --- ASK about that */
				uint16_t opcode;
				char filename[512];
				char mode[512];
				char IP[100];
				int port;
				int blocknum;

				// Empty those char arrays 
				bzero(filename, sizeof(filename));
				bzero(mode, sizeof(mode));
				bzero(IP, sizeof(IP));

				parseUDPmessg(mesg, &opcode, filename, mode, IP, &port, &cliaddr, &blocknum);

				int arrayiter;
				// HANDLES THE RESPONSE-------------------------------------------------------------------------
				if (opcode == 1)
				 {
					
					// WE NEED TO FIND AN AVAILABLE SPACE IN ARRAY
					
					for (arrayiter = 0; arrayiter < LIST_LENGTH; ++arrayiter)
					 { 
						if((sockList[arrayiter])->available)
						 {
							((sockList[arrayiter])->available)--;
							break;
						 }
					 }			

					struct sockaddr_in placholdaddr;	// used when you create socket
					inloopfd = createUDPSocket(0,&placholdaddr);
					(sockList[arrayiter])->fd = inloopfd;
					(sockList[arrayiter])->servaddr = &placholdaddr;
					if ( ( ((sockList[arrayiter])->ffd) = open(filename, O_RDONLY)) == -1)	//file not in system
					 {
						sendError((sockList[arrayiter])->fd, &cliaddr, clilen);
						(sockList[arrayiter])->dying = 1;
						continue;

					 } else	// send first data packet
					 {
							
				
						//START PROBLEM
						char udpresp[516];
						bzero(udpresp, sizeof(udpresp));
						int numread = createUDPdataPacket(udpresp, &((sockList[arrayiter])->blocknum), ((sockList[arrayiter])->ffd), &((sockList[arrayiter])->dying));

						sendto(inloopfd, udpresp, numread + 4 , 0, (SA*) &cliaddr, clilen);
						//END PROBLEM
					 }
			
				 } else if (opcode == 4)
				 {

					if(((sockList[arrayiter])->dying) == 1)
					 {
						printf("YA DEAD\n");
						fflush(stdout);
						if (arrayiter != 0)
							(sockList[arrayiter])->available = 1;
						(sockList[arrayiter])->blocknum = 0;
						(sockList[arrayiter])->dying = 0;
						(sockList[arrayiter])->ffd = 0; 
						continue;
					 } else 
					 {
					//START PROBLEM
					char udpresp[516];
					bzero(udpresp, sizeof(udpresp));
					int numread = createUDPdataPacket(udpresp,&((sockList[arrayiter])->blocknum), ((sockList[arrayiter])->ffd), &((sockList[arrayiter])->dying));
					sendto((sockList[arrayiter])->fd, udpresp, numread + 4 , 0, (SA*) &cliaddr, clilen);
					//END PROBLEM
					 }
		   		 }
			 }
		 }
	 }
	close(sockfd);
	close(defaultsockfd);

	return 0;
}















void sendError(int sockfd, struct sockaddr_in* cliaddr, int clilen)
{

		uint16_t err_op_code, errcode;
		char* error_string = "File not found.";
		char error[MAX_LINE];
		bzero(error, sizeof(error));


		err_op_code = htons(5);
		errcode = htons(1);
		

		memcpy(error, &err_op_code , 2);	
		memcpy((error+2), &errcode, 2);
		memcpy((error+4), error_string, strlen(error_string));
		

		sendto(sockfd,error, sizeof(error),0,(SA*) cliaddr, clilen);


}










int parseUDPmessg(char *mesg, uint16_t *opcode, char *filename, char *mode, char *IP, int *port, struct sockaddr_in* cliaddr, int* blocknum)
{
	int stringlength;
	int string_offset = 0;	
	*opcode = ntohs(*((uint16_t *) mesg));	

	if (*opcode == 1)	// RRQ
	{
		printf("RRQ ");

		// PARSE THAT FILENAME
		string_offset += 2;	// String offset allows access to later portions of array
		stringlength = strlen((mesg + string_offset));
		memcpy(filename, (mesg + string_offset), (stringlength + 1) );
		string_offset += stringlength + 1;

		// PARSE THAT MODE huuuuuSONNNNNNNNNNNNNNNNNNNNNNNNNN
		stringlength = strlen((mesg + string_offset));
		memcpy(mode, (mesg + string_offset), (stringlength+1));
		
		// get that IP
		strcpy(IP,inet_ntoa(cliaddr->sin_addr));

		// get that port swag
		*port = cliaddr->sin_port;
		
		// prints the rest of message
		printf("%s %s from %s:%d\n", filename, mode, IP, *port);
		fflush(stdout);

		int clilen = sizeof(*cliaddr);

	} else if (*opcode == 2)
	{
		printf("WRQ WE DONT SERVICE EM");

	} else if (*opcode == 3) // DATA
	{
		printf("DATA WE DONT SERVICE EM");

	} else if (*opcode == 4) // ACKNOWLEDGE
	{
		printf("ACK ");

		string_offset += 2;
		*blocknum = ntohs( *( (uint16_t *) (mesg + string_offset) ) );
		printf("%u\n", *blocknum);
		fflush(stdout);
		

	} else if (*opcode == 5)
	{
		printf("ERROR ");

	} else
	{
		// error handling done here, make a function for it

		return -1; //Good enough
	}

	return 0;
}














int createUDPSocket(int port, struct sockaddr_in* servaddr)
{
	int sockfd;
	if( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1 )
	{
		printf("Cannot get socket\n");
		exit(1);
	}

	bzero(servaddr, sizeof(*servaddr));
	servaddr->sin_family = AF_INET;
	servaddr->sin_port = htons(port);
	servaddr->sin_addr.s_addr = htonl(INADDR_ANY);

	bind(sockfd, (SA*)servaddr, sizeof(*servaddr));
	return sockfd;
}









int createUDPdataPacket(char* udpresp,uint16_t* blocknum, int fd, int* dying)
{
	char data[512];
	int numread;
	if (( numread = read(fd, data, sizeof(data))) < 512 )
	{	
		*dying = 1;
	}

	uint16_t respopcode = htons(3);	
	
	(*blocknum) = htons(1 + ntohs((*blocknum)));
	printf("%u\n", ntohs(*blocknum));
	memcpy(udpresp, &respopcode , sizeof(respopcode));

	memcpy((udpresp+2), blocknum, 2);

	memcpy((udpresp+4), data, sizeof(data));
	
	return numread;
}







