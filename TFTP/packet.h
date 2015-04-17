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

typedef struct state State;
typedef struct error Error;
typedef struct request Request;
