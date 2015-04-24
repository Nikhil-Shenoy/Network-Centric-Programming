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
	int offset;
	uint16_t blockNumber;
	uint16_t opcode;
};

typedef struct error Error;
typedef struct request Request;
typedef struct state State;
