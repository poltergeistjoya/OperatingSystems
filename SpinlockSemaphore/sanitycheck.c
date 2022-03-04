#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#include "spinlock.h"

int main(int argc, char *argv[]){

	if(argc != 3){
		fprintf(stderr, "Specify the number of processes and number of iterations\n");
		exit(EXIT_FAILURE);
	
	}

	long long unsigned int NumProc = atoll(argv[1]), NumItr = atoll(argv[2]);
	fprintf(stderr, "You specified %llu processes\n", NumProc);
	fprintf(stderr, "You specified %llu iterations\n", NumItr);

	//shared memory region
	int *nonTASregion = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
	if(nonTASregion == MAP_FAILED){
		fprintf(stderr, "Mmap Error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	int *TASregion = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
	if(TASregion == MAP_FAILED){
		fprintf(stderr, "Mmap Error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	nonTASregion[0] = 0;
	TASregion[0] = 0;

	//lock region
	char *lock = (char *)mmap(NULL, sizeof(char), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if(lock == MAP_FAILED){
		fprintf(stderr, "Mmap Error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	int process[NumProc];

	//fork
	int i;
	for(i = 0; i < NumProc; i++){
		if((process[i] = fork()) < 0){
			fprintf(stderr, "Fork Error: %s, process: %d\n", strerror(errno), i);
			return EXIT_FAILURE;
		}
		else if(process[i] == 0){
			int j;
			for(j = 0; j < NumItr; j++){
				nonTASregion[0]++;
			}
			spin_lock(lock);
			int k;
			for(k = 0; k < NumItr; k++){
				TASregion[0]++;
			}
			spin_unlock(lock);
			return 0;

		}
	}
	int l;
	for(l = 0; l < NumProc; l++){
		if(waitpid(process[l], NULL, 0) < 0){
			fprintf(stderr, "Waitpid Error: %s\n", strerror(errno));
		}

	}

	fprintf(stderr, "Expected: %llu\n", NumProc * NumItr);
	fprintf(stderr, "Without Locking: %llu\n", *nonTASregion);
	fprintf(stderr, "With Locking: %llu\n", *TASregion);


	return 0;
}
