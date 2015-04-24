#include <stdint.h>


#define MAXLINE 2048
struct request {
	char buffer[MAXLINE];
	uint16_t opcode;
	char filename[MAXLINE];
	char mode[9];
};

struct error {
	uint16_t opcode;
	char errorCode[2];
	char ErrMsg[MAXLINE];
};

struct state {
	unsigned char Data[512];
	unsigned char Mode[MAXLINE];
	unsigned char Filename[MAXLINE];
	uint16_t BlockNum;
	uint16_t Opcode;
	int Offset;
	int BytesRead;
};

struct conn {
	uint16_t blockNum;
	int clifd;
	int lastPacket; // 1 for yes, 0 for no. dying
	int open; // 1 for yes, 0 for no. available
	int offset;
	unsigned char filename[MAXLINE];
	unsigned char oldData[512];
	struct sockaddr_in *clientInfo;
	struct sockaddr_in *serverInfo;
	
};

	
typedef struct state State;
typedef struct error Error;
typedef struct request Request;
typedef struct conn Conn;
