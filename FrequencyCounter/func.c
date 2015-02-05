#include <string.h>
#include <stdlib.h>
#include <errno.h>
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
	perror("Error");
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
                if(c == string[0]) { 
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

                        fseek(source,-(strlen(string)-1),SEEK_CUR); 
		}
	}


	// Return stream position indicator to beginning of the file
        fseek(source,0,SEEK_SET);
        return counter;
}

