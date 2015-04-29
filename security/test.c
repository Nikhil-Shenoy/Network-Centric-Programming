#include <stdio.h>


int main(int argc, char **argv) {


	char sup[20] = "Hello World!\n";
	printf("%s",sup+2);
	
	printf("Getting something from stdin: ");
	char response[1000];
	fgets(response,1000,stdin);
	
	printf("The response is: %s\n",response);

	snprintf(sup,20,"%s",response);
	printf("Sup is now: %s\n",sup);

	FILE *fp;
	fp = fopen("somecrap.txt","r");
	if(fp == NULL)
		fprintf(stderr,"Cannot create a file with fopen when using \"r\" flag\n");

	return 0;
}
