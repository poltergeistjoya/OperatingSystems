#define _GNU_SOURCE
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

//handler will print the signal that is recieved
void h(int signum){
	fprintf(stderr, "Signal number recieved: %d\n", signum);
	exit(0);
}

int main(){
	//MAP_ANON bc mapping to memory region w no file associated
	char *addr = mmap(NULL, sysconf(_SC_PAGE_SIZE), PROT_READ, MAP_ANONYMOUS|MAP_PRIVATE, 0, 0); //map region with PROT_READ
	if(addr == MAP_FAILED){
		fprintf(stderr, "mmap fail: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	struct sigaction sa;
		sa.sa_flags = 0;
		sa.sa_handler = h;
		sigemptyset(&sa.sa_mask);
	//we loop through all the possible signals, and if the signal was recieved then redirected to handler
	int i;
	for(i = 1; i<32; i++){   
		sigaction(i, &sa, NULL);
	}	
	//write to the memory region
	fprintf(stderr, "Attempting to write to read only memory\n");
	addr[1] = 'X';
	//255 if fail, 0 if suceed
	if(addr[1]!= 'X'){
		return 255; 
	}
	else{
		return 0;
	}
}
