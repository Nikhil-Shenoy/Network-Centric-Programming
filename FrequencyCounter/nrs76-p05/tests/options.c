#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

void hello() {
	printf("Hello\n");
}

void goodbye() {
	printf("Goodbye\n");
}


int main(int argc, char *argv[]) {

	if(argc < 3) {
		printf("Not enough args\n");
		exit(1);
	}

	const char * const short_options = "s";
	const struct option long_options[] = {
		{"systemcalls",0,NULL,'s'},
		{NULL, 0, NULL, 0}
	};

	int next_option;
	
	do {	
		next_option = getopt_long(argc,argv,short_options, long_options,NULL);

		switch(next_option) {
			case 's':
				hello();
				break;
			default:
				printf("Next_option is: %u\n",next_option);
				goodbye();
				break;
		}
	} while(next_option != -1);

	printf("First non-option argument: %s\n",argv[optind]);
	printf("Second non-option argument: %s\n",argv[optind+1]);


	// Done with options processing


	return 0;
}






