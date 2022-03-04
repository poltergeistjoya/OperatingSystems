#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <fcntl.h>


int main(){

	char c = 'c';
	int fd[2];
	int bytecount = 0;
	int w = 0;	
	int size;
	int error;

	if(pipe(fd) != 0){
		fprintf(stderr, "Failed creating pipe %s.\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	fcntl(fd[1], F_SETFL, O_NONBLOCK);


	while (write(fd[1], &c, 1) != -1){
		bytecount++;
		//fprintf(stderr, "%d bytes written\n", bytecount);
	}
	
	fprintf(stderr, "Error is %i: %s.\n", errno, strerror(errno));
	error = errno;

	if(errno == 11){
		fprintf(stdout, "Errno 11 is EAGAIN for Linux. Failed with expected error.\n");

	}

	fprintf(stdout, "%d total bytes written into pipe.\n", bytecount);

	size = fcntl(fd[1], F_GETPIPE_SZ);
	fprintf(stdout, "Expected pipe size of %d.\n", size);
	
	if(bytecount == size){
		fprintf(stdout, "Expected pipe size matches measured pipe size.\n");
	}
	
	if((bytecount == size) && (error == 11)){
		fprintf(stdout, "Experiment run successfully! Woo!\n");
	}

	return 0;
}

