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


struct threadPackage {
	FILE *log;
	struct sockaddr_in clientAddress;
	int clientLen;
	char browserRequest[MAXLINE];
	char browserHost[MAXLINE];
	rio_t Rio;
	rio_t Rio2;
	int browserFd;
};


void *worker(void *tp) {


	printf("Inside the worker\n");
	int serverfd, clientPort;
	char *clientIP;
	char URL[MAXLINE];
	char hostString[MAXLINE];	
	char host[MAXLINE];
	int logMessageLength;
	char logMessage[logMessageLength];
	char request[MAXLINE];
	char requestType[10];
	char response[MAXLINE];
       	char browserReq[MAXLINE];
        ssize_t bytesRead;
	FILE *logfile;
	rio_t rio, rio2;
	int browserfd;

	struct sockaddr_in clientAddr;


	struct threadPackage *TP;

	TP = (struct threadPackage *)tp;

	
	browserfd = TP->browserFd;
	Rio_readinitb(&rio,browserfd);
	strcpy(browserReq,TP->browserRequest);
	strcpy(hostString,TP->browserHost);	
	
		// Get the client IP using the clientaddr struct
		printf("Getting client IP\n");
		clientIP = inet_ntoa((TP->clientAddress).sin_addr);
	
		char *colon;
		int colonCount;
		colonCount = 0;
		colon = browserReq;
		while((colonCount != 2) && (*colon != '\0')){
			if(*colon == ':')
				colonCount++;
	
			colon++;
		}
		
		if(isdigit(*(colon+1))) {
			char specialPort[10];
			int j;
			j = 0;
			while(*colon != '/') {
				specialPort[j] = *colon;
				j++;
				colon++;
			}

			clientPort = atoi(specialPort);
		}
		else {
			sscanf(browserReq,"%s %s HTTP/1.1\r\n",requestType,URL); // got URL
sscanf(browserReq,"%s %s HTTP/1.1\r\n",requestType,URL); // got URL
			clientPort = 80;
		}

	
		sscanf(hostString,"Host: %s\r\n",host); // got host
		printf("Got the host\n");	
			

			
		printf("Going to try Open_clientfd\n");
	        serverfd = Open_clientfd(host,clientPort);

	        printf("Opened connection to host\n");
	        Rio_readinitb(&rio2,serverfd);
	        printf("Ready to write to client file descriptor\n");
	
		// Form HTTP/1.0 Request
		sprintf(request,"%s %s HTTP/1.0\r\nHost: %s\r\n\r\n",requestType,URL,host);
	
	
		printf("Proxy sent request to host\n");
	
		// Send request to server
	        Rio_writen(serverfd,request,strlen(request));

		logMessageLength = 40 + strlen(clientIP) + strlen(URL);
		char contentLength[15];
		char *contentPtr;

		// NEED TO LOCK THE LOG FILE

		// Retrieve response from the server and forward to browser
		while((bytesRead = Rio_readlineb(&rio2,response,MAXLINE)) > 0) {
			if((contentPtr = strstr(response,"Content-Length: ")) != NULL) {
				sscanf(response,"Content-Length: %s\r\n",contentLength);
				format_log_entry(logMessage,&clientAddr,URL,atoi(contentLength)); 
				fprintf(logfile,"%s\n",logMessage); // print log entry

			}


			Rio_writen(browserfd,response,bytesRead);
		}
		
		//fclose(logfile);

	pthread_exit(NULL);

}


int main(int argc, char **argv)
{

	int listenfd, port;
	rio_t rio,rio2;

	int browserfd, serverfd, clientlen, bytesRead;
        struct sockaddr_in clientaddr;
	char buffer[MAXLINE];
	char browserHost[MAXLINE];

	/* Check arguments */
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <port number>\n", argv[0]);
		exit(0);
	}


	
	port = atoi(argv[1]); // Tested using port 5000
	printf("Proxy started on port %u. Waiting for client connection...\n",port);

	listenfd = Open_listenfd(port); // Creates a socket and binds it to the given port.

	while(1) {

		FILE *logfile = fopen("requestLog.txt","a");

		clientlen = sizeof(struct sockaddr_in);

	
		printf("Server is listening for client connection requests...\n");
	
	
	        // Listens for requests on the listenfd, fills in client socket address in addr, returns 
	        // connected descriptor for communication with client
	        browserfd = Accept(listenfd,(SA *)&clientaddr, &clientlen);
	
		Rio_readinitb(&rio,browserfd);

	        // Let's read a request from the browser into the buffer
	        bytesRead = Rio_readlineb(&rio,buffer,MAXLINE);
		bytesRead = Rio_readlineb(&rio,browserHost,MAXLINE);

		// package for the thread
		struct threadPackage tp;
		tp.log = logfile;
		tp.clientAddress = clientaddr;
		tp.clientLen = clientlen;
		strcpy(tp.browserRequest,buffer);
		tp.Rio = rio;
		tp.Rio2 = rio2;
		tp.browserFd = browserfd;
		strcpy(tp.browserHost,browserHost);

		pthread_t tid;
		int threadReturn;
		threadReturn = pthread_create(&tid,NULL,worker,(void *)&tp);
		pthread_join(tid,NULL);

		printf("Done with the sending\n");

	}
	printf("Exited the while loop\n");
	close(browserfd);
	close(serverfd);	

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
    sprintf(logstring, "%s: %d.%d.%d.%d %s %d", time_str, a, b, c, d, uri, size);
}


