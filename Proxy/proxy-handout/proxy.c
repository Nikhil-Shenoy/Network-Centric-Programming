/*
 * proxy.c - A Simple Sequential Web proxy
 *
 * Course Name: 14:332:456-Network Centric Programming
 * Assignment 2
 * Student Name:______________________
 * 
 * IMPORTANT: Give a high level description of your code here. You
 * must also provide a header comment at the beginning of each
 * function that describes what that function does.
 */ 

#include "csapp.h"

/*
 * Function prototypes
 */
void format_log_entry(char *logstring, struct sockaddr_in *sockaddr, char *uri, int size);

/* 
 * main - Main routine for the proxy program 
 */


char *getURL(char *buffer) {

	char *requestTokens[10];
        char *token;
        token = strtok(buffer," ");
        int i;
        i = 0;
        while((i != 10) && (token != NULL)) {
                requestTokens[i] = token;
                //printf("requestTokens[%u] = %s\n",i,requestTokens[i]);
                i++;
                token = strtok(NULL," ");
        }

        // Find the URL
        i = 0;
        while(strcmp("GET",requestTokens[i]) != 0)
                i++;

        // NEED TO GET THE PORT, IF AVAILABLE


        char *URL;
        URL = requestTokens[i+1];
        return URL;



}


	



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



int main(int argc, char **argv)
{

	int listenfd, connfd, port, clientlen;
        struct sockaddr_in clientaddr;


	/* Check arguments */
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <port number>\n", argv[0]);
		exit(0);
	}

	
	port = atoi(argv[1]);
	printf("Proxy started on port %u. Waiting for client connection...\n",port);
        listenfd = Open_listenfd(port); // Creates a socket and binds it to the given port.

        clientlen = sizeof(clientaddr);
	FILE *logfile = fopen("clientLog.txt","w");

	printf("Server is listening for client connection requests...\n");

        // Listens for requests on the listenfd, fills in client socket address in addr, returns 
        // connected descriptor for communication with client
        connfd = Accept(listenfd,(SA *)&clientaddr, &clientlen);
       	char buffer[501];
	char request[501];
        ssize_t bytesRead;

	//printf("The buffer is: %s\n",buffer);	
        // Let's read a request into the buffer
        bytesRead = Rio_readn(connfd,buffer,500);
	//printf("The buffer is: %s\n",buffer);	
	strcpy(request,buffer);

	// Get the client IP using the clientaddr struct
	char *clientIP;
	clientIP = inet_ntoa(clientaddr.sin_addr);

	printf("The client's IP is: %s\n",clientIP);

	// GET HOST FROM THE BUFFER
	/*
		1) Get a pointer to the line that you need
		2) Count the number of characters between The beginning and the \r
		3) Move the pointer forward by 6 bytes
		4) Subtract 6 from the character count and read the characters into an array
	*/

	char *hostPtr, *countPtr;
	hostPtr = strstr(buffer,"Host: ");
	int count;
	count = 0;
	countPtr = hostPtr;
	while(*countPtr != '\r') {
		countPtr++;
		count++;
	}
		
	hostPtr += 6;
	char host[count-5];
	int counter = 0;
	while(*hostPtr != '\r') {
		host[counter] = *hostPtr;
		counter++;
		hostPtr++;
	}

	host[count-6] = '\0';
	char *URL;
	URL = getURL(buffer);

	int logMessageLength;
	logMessageLength = 30 + strlen(clientIP) + strlen(URL);
	char logMessage[logMessageLength];
	format_log_entry(logMessage,&clientaddr,URL,0);
	//printf("The logMessage is: %s\n",logMessage);

	fprintf(logfile,"%s\n",logMessage);
	//printf("Using host (%s) and bytesRead (%u)\n",host,bytesRead);

	//printf("Enter the word \"end\" to end the request cycle: ");


	fclose(logfile);

/////////////////////////////////////////////////////////////////////////////////////////////

	int clientfd, clientPort;
	rio_t rio;

	clientPort = 80;
	printf("Going to try Open_clientfd\n");
        clientfd = Open_clientfd(host,clientPort);
        printf("Opened connection to host\n");
        Rio_readinitb(&rio,clientfd);
        printf("Ready to write to client file descriptor\n");

	// Create request to server
	/*
	char *endRequest;
	endRequest = strstr(request,"User-Agent:");
	*endRequest = '\r';
	*(endRequest+1) = '\n';
	*(endRequest+2) = '\0';
	*/
	char initialResponse[MAXLINE];	
	printf("Proxy sent request to host\n");
        Rio_writen(clientfd,request,strlen(request));

	char Response[1000];
	while((bytesRead = read(clientfd,Response,1000)) > 0) 
		write(connfd,Response,bytesRead);

	printf("Done with the sending\n");


	
	

	/*	
        read(clientfd,initialResponse,MAXLINE); 
	// Only read gets the actual content. Rio_readlineb does not 
	printf("Proxy received response from host\n");
                // Make into findContentLength()

        int contentLength;
        contentLength = findContentLength(initialResponse);


        // Create the response in one large character array
//      char response[strlen(buf) + contentLength];
	printf("Making space for the response...\n");
        char *response = (char *)malloc((strlen(initialResponse) + contentLength)*sizeof(char));
	//char *response = (char *)malloc(10000000*sizeof(char));
        strcpy(response,initialResponse);

        char *end;
        end = NULL;

	printf("Constructing response for client...\n");
        while(end == NULL) {
                read(clientfd,initialResponse,MAXLINE);
                end = strstr(initialResponse,"</html>");
                strcat(response,initialResponse);
        }
	printf("Constructed response for client\n");
	*/
//	write(connfd,Response,strlen(Response));
//	Rio_writen(connfd,response,strlen(response));
	//end = strstr(response,"</html>");

	close(connfd);
	close(clientfd);	

	//strcat(response,"\r\n\r\n\0");

 //       printf("Statement is: %s\n\n",response);
//        free(response);



	exit(0);
}

/*
 * format_log_entry - Create a formatted log entry in logstring. 
 * 
 * The inputs are the socket address of the requesting client
 * (sockaddr), the URI from the request (uri), and the size in bytes
 * of the response from the server (size).
 */
void format_log_entry(char *logstring, struct sockaddr_in *sockaddr, 
		      char *uri, int size)
{
    time_t now;
    char time_str[MAXLINE];
    unsigned long host;
    unsigned char a, b, c, d;

    /* Get a formatted time string */
    now = time(NULL);
    strftime(time_str, MAXLINE, "%a %d %b %Y %H:%M:%S %Z", localtime(&now));

    /* 
     * Convert the IP address in network byte order to dotted decimal
     * form. Note that we could have used inet_ntoa, but chose not to
     * because inet_ntoa is a Class 3 thread unsafe function that
     * returns a pointer to a static variable (Ch 13, CS:APP).
     */
    host = ntohl(sockaddr->sin_addr.s_addr);
    a = host >> 24;
    b = (host >> 16) & 0xff;
    c = (host >> 8) & 0xff;
    d = host & 0xff;


    /* Return the formatted log entry string */
    sprintf(logstring, "%s: %d.%d.%d.%d %s", time_str, a, b, c, d, uri);
}


