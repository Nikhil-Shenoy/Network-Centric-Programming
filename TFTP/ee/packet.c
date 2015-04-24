#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "packet.h"

void constructRequest(Request *newRequest,uint16_t Opcode, char *Filename, char *Mode) {

	
	// set everything to null
	memset(newRequest,'\0',sizeof(*newRequest));
	if(Opcode == 1 || Opcode == 2)
		newRequest->opcode = Opcode;
	else {
		fprintf(stderr,"Opcode is not valid\n");
		exit(1);
	}

	if(strlen(Filename) > 2048) {
		fprintf(stderr,"Filename exceeds preset size (2048 chars)\n");
		exit(1);
	} else
		strcpy(newRequest->filename,Filename);

	
	int comp1, comp2, comp3;
	comp1 = strcmp(Mode,"netascii");
	comp2 = strcmp(Mode,"octet");
	comp3 = strcmp(Mode,"mail");
	
	// compN will be zero if there is a match
	// If any one of the negations is 1, then we copy the mode in
	// Else, we get an error

	if(!comp1 || !comp2 || !comp3) {
		strcpy(newRequest->mode,Mode);
	} else {

		fprintf(stderr,"Mode exceeds preset size (10 chars)\n");
		exit(1);
	}

}	
