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
#include <stdint.h>
#include "packet.h"


void printRequest(Request *request, struct sockaddr_in *cliPtr);
int organizeRequest(Request *newRequest);
int sendErrorPacket(int sockfd, struct sockaddr_in client, socklen_t clilen);
int checkForFile(char *filename);
int sendDataPacket(int sockfd, struct sockaddr_in client, socklen_t clilen, char *filename, char *mode, int resend);
