#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <ctype.h>

int main(int argc, char *argv[]) {

	int fd;
	fd = open("b.txt",O_RDONLY);

	int i,j;	

	char c;

	do {
		j = read(fd,&c,1);
		printf("The character is: %c\n",c);
	} while( (j != 0) && (isalnum(c) != 0));
	close(fd);

	return 0;
}	
