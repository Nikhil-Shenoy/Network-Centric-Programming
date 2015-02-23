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

	char prompt[50];
	*prompt = "prompt";	
	while(strcmp(prompt,"end") != 0) {
		printf("Server is listening for client connection requests...\n");

	        // Listens for requests on the listenfd, fills in client socket address in addr, returns 
	        // connected descriptor for communication with client
	        connfd = Accept(listenfd,(SA *)&clientaddr, &clientlen);
        	char buffer[501];
	        ssize_t bytesRead;
	
	        // Let's read a request into the buffer
	        bytesRead = Rio_readn(connfd,buffer,500);
	
	
		// Get the client IP using the clientaddr struct
		char *clientIP;
		clientIP = inet_ntoa(clientaddr.sin_addr);
	
		printf("The client's IP is: %s\n",clientIP);
	
		// Retrieve tokens of the request from the buffer
		// Used an array of 10 since the URL should appear within that many tokens

		char *requestTokens[10];
		char *token;
		token = strtok(buffer," ");
		int i;
		i = 0;
		while((i != 10) && (token != NULL)) {
			requestTokens[i] = token;
			printf("requestTokens[%u] = %s\n",i,requestTokens[i]);
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

		int logMessageLength;
		logMessageLength = 30 + strlen(clientIP) + strlen(URL);
		char logMessage[logMessageLength];
		format_log_entry(logMessage,&clientaddr,URL,0);
		printf("The logMessage is: %s\n",logMessage);
		fprintf(logfile,"%s\n",logMessage);


		printf("Enter the word \"end\" to end the request cycle: ");
		scanf("%s",prompt);
	}
	
	fclose(logfile);

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


