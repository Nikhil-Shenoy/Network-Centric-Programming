#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <netdb.h>


int main(int argc, char *argv[]) {

	char *host;
	int port;

	if(argc != 3) {
		fprintf(stderr,"Usage error\n");
		exit(1);
	}

	host = argv[1];
	port = atoi(argv[2]);


	char buffer[2048];
	while(fgets(buffer,2048,stdin) != NULL) {
		int connFd;
		connFd = socket(AF_INET, SOCK_STREAM,0);

		struct hostent *hp;
		hp = gethostbyname(host);
	

		struct sockaddr_in serveraddr; // this should be filled in with the server information given from the client
		bzero(&serveraddr,sizeof(serveraddr));
		serveraddr.sin_family = AF_INET;
		bcopy((char *)hp->h_addr_list[0],(char *)&serveraddr.sin_addr.s_addr,hp->h_length);
		serveraddr.sin_port = htons(port);
	
		connect(connFd,(struct sockaddr *)&serveraddr,sizeof(serveraddr));
		read(connFd,buffer,2048); // get the response from the server
		printf("Response is: %s\n",buffer);
		close(connFd);
	}


}






