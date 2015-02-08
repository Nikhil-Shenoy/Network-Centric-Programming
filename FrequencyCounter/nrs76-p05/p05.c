#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>
#include <fcntl.h>
#include "include/func.h" // contains custom auxiliary functions 



int main(int argc, char *argv[]) {

	// Check to see if at least three arguments have been provided

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
					fprintf(stderr,"Usage: ./counter <source file> <substring 1> <substring 2> ...\n\t--system calls flag is optional\n");
					exit(1);
				} else
					sysFlag = 1;
				break;
			default:
				if(argc < 3) {
					fprintf(stderr,"Not enough arguments\n");
					fprintf(stderr,"Usage: ./counter <source file> <substring 1> <substring 2> ...\n\t--system calls flag is optional\n");

					exit(1);
				} 
				break;
		}
	} while(next_option != -1);


	FILE *source;
	source = NULL;
	int fileDescriptor;

	// System call flag is used, use open() system call to open the file. Else, use fopen
	if(sysFlag == 1) {
		fileDescriptor = open(argv[optind],O_RDONLY); 
		if(fileDescriptor == -1) {
			perror("./p05");
			exit(1);
		}

	} else {
		source = fopen(argv[optind],"r");
		// Check for file opening error
		if(source == NULL) {
			perror("./p05");
			exit(1);
		}
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
	

	// Perform frequency count on each substring. Print the count and then process next string

	if(sysFlag != 1) {
		for(i = 0; i < stringArgs; i++) {
			count = freqCount(source,substrings[i]);
			printf("%u\n",count);
		}	
	

		// Close the source file
		fclose(source);
	} else {
		for(i = 0; i < stringArgs; i++) {
			count = sysCallCount(fileDescriptor,substrings[i]);
			printf("%u\n",count);
		}
		// Close the source file
		close(fileDescriptor);
	}
	return 0;
}
