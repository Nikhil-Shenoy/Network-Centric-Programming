#include "csapp.h"

int main(int argc, char **argv) {

	int clientfd, port;
	char *host, buf[MAXLINE];
	rio_t rio;

	if(argc != 3) {
		fprintf(stderr,"usage: %s <host> <port>\n",argv[0]);
		exit(0);
	}

	host = argv[1];
	port = atoi(argv[2]);

	printf("Going to try Open_clientfd\n");
	clientfd = Open_clientfd(host,port);
	printf("Opened connection to host\n");
	Rio_readinitb(&rio,clientfd);
	printf("Ready to write to client file descriptor\n");


	char *request;
	request = "GET / HTTP/1.1\r\nHost: www.google.com\r\n";
	while(Fgets(buf,MAXLINE,stdin) != NULL) {
		Rio_writen(clientfd,request,strlen(request));
		//Rio_readlineb(&rio,buf,MAXLINE);
		read(clientfd,buf,MAXLINE);
		
		printf("Statement is: %s\n\n",buf);
	}

	printf("cool\n");

	return 0;
}	
