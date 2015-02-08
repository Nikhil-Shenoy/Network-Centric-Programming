#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>

int main() {

	int fd;
	fd = open("../sample.txt",O_RDONLY);

	char c[1];
	char buffer[5];
	buffer[4] = '\0';

	ssize_t readVal;
	readVal = read(fd,buffer,4);
	printf("First buffer is: %s\n",buffer);
	lseek(fd,-1,SEEK_CUR);
	readVal = read(fd,buffer,4);
	printf("Second buffer is: %s\n",buffer);
	
	return 0;
}
