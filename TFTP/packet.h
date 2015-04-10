#include <stdint.h>

#define MAXLINE 2048
struct request {
	char buffer[MAXLINE];
	uint16_t opcode;
	char filename[MAXLINE];
	char mode[9];
};

typedef struct request Request;
