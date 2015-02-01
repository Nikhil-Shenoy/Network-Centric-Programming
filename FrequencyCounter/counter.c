#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "include/func.h"

/*
char *lowerCase(char *arg) {

	int i;
	i = 0;

	while(i != strlen(arg)) {
		arg[i] = tolower(arg[i]);
		i++;
	}

	return arg;
}


// This function counts the frequency of a given string in the given file.
int freqCount(FILE *source, char *string) {

	int counter;
	counter = 0;
	char buffer[strlen(string) + 1]; // space to hold candidates from the file
	
	char c;
	while( (c = fgetc(source)) != EOF) { // Get character from file
		if(c == string[0]) { // If starting characters match, begin the processing
			fseek(source,-1,SEEK_CUR);
			fgets(buffer,strlen(string)+1,source); // get the candidate string from file
			if(strcmp(lowerCase(buffer),string) == 0) // If the strings match, increment the counter
				counter++;
			fseek(source,-(strlen(string)-1),SEEK_CUR); // Reposition stream position to next character after the matched character

		}
	}


	fseek(source,0,SEEK_SET); // Return stream position indicator to beginning of the file
	return counter;
}	
*/


int main(int argc, char *argv[]) {

	if(argc < 3) {
		printf("Error: Not enough arguments\n");
		printf("Usage: ./counter <source file> <substring 1> <substring 2> ...\n");
		return 1;
	}

	FILE *source = fopen(argv[1],"r");
	if(source == NULL) {
		perror("Error with given file");
		return 1;
	}

	int stringArgs, i, count;
	stringArgs = argc-2;
	char *substrings[stringArgs];

	for(i = 2; i < argc; i++) 
		substrings[i-2] = lowerCase(argv[i]);
	

	for(i = 0; i < stringArgs; i++) {
		count = freqCount(source,substrings[i]);
		printf("%u\n",count);
	}	

	fclose(source);
	return 0;
}
