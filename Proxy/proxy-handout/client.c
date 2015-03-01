#include "csapp.h"

int findContentLength(char *buf) {
	char *contentLength;
	contentLength = strstr(buf,"Content-Length: ");
	contentLength = contentLength + 16; // Move the pointer to the beginning of the number
	char byteString[10] = "";

	int counter;
	counter = 0;	
	while(isdigit(*contentLength) != 0) {
		byteString[counter] = *contentLength;
		contentLength++;
		counter++;
	}	

	int contentSize;
	contentSize = atoi(byteString);	

	return contentSize;
}



int main(int argc, char **argv) {

	int clientfd, port;
	char *host; 
	char buf[MAXLINE] = "GET /News/Tennis/2015/02/8/Dubai-Final-Federer-Djokovic.aspx HTTP/1.1\r\nHost: www.atpworldtour.com\r\n\r\n";
	
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


//	char request[MAXLINE] = "GET / HTTP/1.1\r\nHost: www.google.com\r\n\r\n";
	Rio_writen(clientfd,buf,strlen(buf));
	//Rio_readnb(&rio,buf,MAXLINE);
	read(clientfd,buf,MAXLINE); // Only read gets the actual content. Rio_readlineb does not

	/*
		We extract the content size from the initial response. This will be in bytes.
		Since we know how many bytes are in the content, we can create a char array of the same size to send the information back.
	*/

	// Make into findContentLength()

	int contentLength;
	contentLength = findContentLength(buf);


	// Create the response in one large character array
//	char response[strlen(buf) + contentLength];
	char *response = (char *)malloc((strlen(buf) + contentLength)*sizeof(char));
	strcpy(response,buf);

	char *end;
	end = NULL;	

	while(end == NULL) {
		read(clientfd,buf,MAXLINE);
		end = strstr(buf,"</html>");	
		strcat(response,buf);
	}
	
	printf("Statement is: %s\n\n",response);
	free(response);

	return 0;
}	
