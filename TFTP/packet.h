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

typedef struct error Error;
typedef struct request Request;
