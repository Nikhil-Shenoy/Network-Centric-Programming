#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include "include/func.h"


// This function converts all the characters in a string to lowercase
char *lowerCase(char *arg) {

        int i;
        i = 0;

        while(i != strlen(arg)) {
                arg[i] = tolower(arg[i]);
                i++;
        }

        return arg;
}


// Error function
void error() {
	perror("./p05");
	exit(1);
}


// This function counts the frequency of a given string in the given file.
int freqCount(FILE *source, char *string) {

        int counter, seekError;
	char *getsError;
        counter = 0;

	// space to hold candidates from the file
        char buffer[strlen(string) + 1]; 

        char c;
	// Get characters from file
        while( (c = fgetc(source)) != EOF) { 
		
		// If starting characters match, begin the processing
                if(tolower(c) == string[0]) { 
                        seekError = fseek(source,-1,SEEK_CUR);
			if(seekError != 0)
				error();
	
			// get the candidate string from file
                        getsError = fgets(buffer,strlen(string)+1,source);
			if(getsError == NULL)
				error();

			// If the strings match, increment the counter
                        if(strcmp(lowerCase(buffer),string) == 0) 
                                counter++;

			// Reposition stream position to next character after the matched character
                        seekError = fseek(source,-(strlen(string)-1),SEEK_CUR); 
			if(seekError != 0)
				error();
		}
	}


	// Return stream position indicator to beginning of the file
        seekError = fseek(source,0,SEEK_SET);
	if(seekError != 0)
		error();


        return counter;
}

// This function counts the frequency of a given string in the given file using system calls

int sysCallCount(int fd, char *string) {


	int counter, seekError,readVal;
	counter = 0;

	char c[1]; // space to read each character
	
	// Space to hold candidates from the file
	char buffer[strlen(string)+1];
	buffer[strlen(string)] = '\0';


	readVal = read(fd,c,1);
	if(readVal == -1)
		error();

	while (readVal != 0){
		
		// If starting characters match, begin the processing
		if(tolower(*c) == string[0]) {
			seekError = lseek(fd,-1,SEEK_CUR);
			if(seekError == -1)
				error();
	
			// get the candidate string from file	
			readVal = read(fd,buffer,strlen(string));
			if(readVal == -1)
				error();


			// If the strings match, increment the counter
			if(strcmp(lowerCase(buffer),string) == 0)
				counter++;

			// Reposition stream position to next character after the matched character
			seekError = lseek(fd,-(strlen(string)-1),SEEK_CUR);
			if(seekError == -1)
				error();

		}
		readVal = read(fd,c,1); // get the next character
		if(readVal == -1)
			error();

	}

	// Return stream position indicator to the beginning of the file	
	seekError = lseek(fd,0,SEEK_SET);
	if(seekError == -1)
		error();

	return counter;
}	






