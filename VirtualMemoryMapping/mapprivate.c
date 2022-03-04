#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

int main(){
	int fd;
	fd = open("./file.txt", O_RDWR|O_CREAT|O_TRUNC, 0666);
	//fd = open("./file.txt", O_RDWR);
	//fprintf(stderr, "%d\n", fd);
	if(fd < 0){
		fprintf(stderr, "open failed with error:%s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	if(write(fd, "b", 1) != 1){
		fprintf(stderr, "write failed\n");
		exit(EXIT_FAILURE);
	}
	struct stat sb;
	if(fstat(fd, &sb) == -1){
		fprintf(stderr, "fstat failed\n");
		exit(EXIT_FAILURE);
	}
	char *addr;
	addr = mmap(NULL, sb.st_size, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0);
	if(addr == MAP_FAILED){
		fprintf(stderr, "mmap failed\n");
		exit(EXIT_FAILURE);
	}
	addr[0] = 'c';
        lseek(fd, 0, SEEK_SET);
	char buf[1];
	int buf_size = sizeof(buf);
	if(read(fd, buf, buf_size) == -1){
		fprintf(stderr, "read failed\n");
		exit(EXIT_FAILURE);
	}
	if(buf[0] == 'c'){
		printf("Update is visible when accessing the file with read\n");
		return 0;
	}
	else if(buf[0] == 'b'){
		printf("Update is not visible when accessing the file with read\n");
		return 1;
	}
	else{
		fprintf(stderr, "Hmm, something went wrong\n");
		exit(EXIT_FAILURE);
	}
}
