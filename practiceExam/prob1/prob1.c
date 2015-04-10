#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {

	char *response = "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n";

	//write(1,response,strlen(response));

	/*
		1) Get the size of the file
		2) Creat a buffer large enough to hold the contents of the file
		3) Copy the contents into the buffer
		4) Create the string
	*/

	char *filename = argv[1];
	struct stat fileInfo;
	stat(filename,&fileInfo);
	int fileSize = fileInfo.st_size;
	
	char content[fileSize];
	int fd;
	fd = open(filename,O_RDWR);
	read(fd,content,fileSize);
	close(fd);

	char *contentLengthStr = "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nContent-Length: ";
	char sizeStr[10];
	sprintf(sizeStr,"%u",fileSize);
	char *termination = "\r\n";

	char header[strlen(contentLengthStr) + strlen(sizeStr) + strlen(termination)];
	sprintf(header,"%s%s%s",contentLengthStr,sizeStr,termination);
	write(1,header,strlen(header));
	 

	return 0;
}
