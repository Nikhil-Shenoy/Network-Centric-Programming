#include <stdio.h>

int main(int argc, char *argv[]) {

	FILE *fp = fopen(argv[1],"r");
	int count; count = 0;

	char c;
	while((c = fgetc(fp)) != EOF)
		count += 1;


	printf("Size of %s is %u\n",argv[1],count);
	fclose(fp);
	
	return 0;
}
