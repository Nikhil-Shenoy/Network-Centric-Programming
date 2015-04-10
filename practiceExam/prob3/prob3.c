#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <strings.h>
#include <time.h>
#include <string.h>

int createListeningSocket(short port) {
	// First create the listenFd
	int listenFd;
	listenFd = socket(AF_INET,SOCK_STREAM,0);
	if(listenFd == -1) {
		perror("socket");
		exit(1);
	}

	// Populate the struct with the server info
	struct sockaddr_in serveraddr;
	bzero(&serveraddr,sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(port);

	// Bind the socket
	int bindVal;
	bindVal = bind(listenFd,(struct sockaddr *)&serveraddr,sizeof(serveraddr));
	if(bindVal == -1) {
		perror("bind");
		exit(1);
	}

	// Listen on the socket
	if(listen(listenFd,1000) == -1) {
		perror("listen");
		exit(1);
	}


	return listenFd;

}


int main(int argc, char *argv[]) {

	int port;
	
	if(argc != 2) {
		fprintf(stderr,"Usage error\n");
		exit(1);
	}

	port = atoi(argv[1]);

	int listenFd;
	listenFd = createListeningSocket(port);
	// Now we can accept connections
	int clientLen, connFd;
	struct sockaddr_in clientaddr; // This gets filled in once the server gets a request
	char buffer[2048];
	for( ; ;) {
		clientLen = sizeof(clientaddr);	
		connFd = accept(listenFd,(struct sockaddr *)&clientaddr,&clientLen);	

		// We've accepted a request. Now we can do the work
		time_t curTime;
		char *timeStr;
		timeStr = ctime(&curTime);

		char buffer[2048];
		
		sprintf(buffer,"%s\n%d\n",timeStr,htonl(time(NULL)));
		write(connFd,buffer,strlen(buffer));
		close(connFd);	
	}

}



	


