#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>
#include "include/func.h" // contains custom auxiliary functions 


int main(int argc, char *argv[]) {

	// Check to see if at least three arguments have been provided
	/*
	if(argc < 3) {
		printf("Error: Not enough arguments\n");
		printf("Usage: ./counter <source file> <substring 1> <substring 2> ...\n");
		return 1;
	}
	*/

        const char * const short_options = "s";
        const struct option long_options[] = {
                {"systemcalls",0,NULL,'s'},
                {NULL, 0, NULL, 0}
        };

        int next_option, sysFlag, argFlag;

	do {
        
	        next_option = getopt_long(argc,argv,short_options, long_options,NULL);

		switch(next_option) {
			case 's':
				if(argc < 4) {
					fprintf(stderr,"Not enough arguments\n");
					fprintf(stderr,"Usage: ./counter <source file> <substring 1> <substring 2> ...\n--system calls flag is optional\n");
					exit(1);
				} else
					sysFlag = 1;
				break;
			default:
				if(argc < 3) {
					fprintf(stderr,"Not enough arguments\n");
					fprintf(stderr,"Usage: ./counter <source file> <substring 1> <substring 2> ...\n--system calls flag is optional\n");

					exit(1);
				} 
				break;
		}
	} while(next_option != -1);


	int k;
	for(k = 0; k < argc; k++) 
		printf("\t%s\n",argv[k]);

	printf("optind is: %u. argc is: %u\n",optind,argc);


	FILE *source = fopen(argv[optind],"r");
	// Check for file opening error
	if(source == NULL) {
		perror("Error with given file");
		fprintf(stderr,"Usage: ./counter <source file> <substring 1> <substring 2> ...\n--system calls flag is optional\n");
		exit(1);
	}

	int stringArgs, i, count, nonSubstrings;
	if(sysFlag == 1) {
		stringArgs = argc-3;
		nonSubstrings = 3;
	} else {
		stringArgs = argc-2;
		nonSubstrings = 2;
	}
	
	// Create an array which holds the user-provided substrings
	char *substrings[stringArgs];
	for(i = optind+1; i < argc; i++) 
		substrings[i-nonSubstrings] = lowerCase(argv[i]);

/*
	if(sysFlag == 1)
		sysCallCount();
	else
		stdioCount();	
*/
	// Perform frequency count on each substring. Print the count and then process next string
	for(i = 0; i < stringArgs; i++) {
		count = freqCount(source,substrings[i]);
		printf("%u\n",count);
	}	

	// Close the source file
	fclose(source);
	return 0;
}
