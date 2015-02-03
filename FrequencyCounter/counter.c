#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>
#include "include/func.h" // contains custom auxiliary functions 


int main(int argc, char *argv[]) {

	// Check to see if at least three arguments have been provided
	if(argc < 3) {
		printf("Error: Not enough arguments\n");
		printf("Usage: ./counter <source file> <substring 1> <substring 2> ...\n");
		return 1;
	}

        const char * const short_options = "s";
        const struct option long_options[] = {
                {"systemcalls",0,NULL,'s'},
                {NULL, 0, NULL, 0}
        };

        int next_option;
        
        next_option = getopt_long(argc,argv,short_options, long_options,NULL);


	FILE *source = fopen(argv[1],"r");
	// Check for file opening error
	if(source == NULL) {
		perror("Error with given file");
		return 1;
	}

	int stringArgs, i, count;
	stringArgs = argc-2;
	
	// Create an array which holds the user-provided substrings
	char *substrings[stringArgs];
	for(i = 2; i < argc; i++) 
		substrings[i-2] = lowerCase(argv[i]);
	

	// Perform frequency count on each substring. Print the count and then process next string
	for(i = 0; i < stringArgs; i++) {
		count = freqCount(source,substrings[i]);
		printf("%u\n",count);
	}	

	// Close the source file
	fclose(source);
	return 0;
}
