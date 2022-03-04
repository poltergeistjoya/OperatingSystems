#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <string.h>

#include "sem.h"

int my_procnum = -1;
int pebbles;
int moves;
int from[] = {0, 0, 1, 1, 2, 2};
int to[] = {1, 2, 0, 2, 0, 1};

int main(int argc, char *argv[]){
	if(argc != 3){
		fprintf(stderr, "Specify the number of pebbles and moves\n./shellgame.exe <pebbles> <moves>\n");
		exit(EXIT_FAILURE);
	}
	pebbles = atoi(argv[1]);
	moves = atoi(argv[2]);
	if(pebbles <= 0 || moves <= 0){
		fprintf(stderr, "Values must be positive\n");
	}

	//initialize three semaphore shells
	struct sem *shell = (struct sem*) mmap(NULL, sizeof(struct sem)*3, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, 0, 0);
	if(shell == MAP_FAILED){
		fprintf(stderr, "Mmap Error: %s\n", strerror(errno));
	}

	int my_pid;
	int pid = getpid();

	int i;
	for(i = 0; i < 3; i++){
		sem_init(shell + i, pebbles);
	}

	//spawn tasks
	for(i = 0; i < 6 && pid != 0; i++){
		pid = fork();
		if(pid == -1){
			fprintf(stderr, "Fork Error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		if(pid == 0){
			my_pid = getpid();
			my_procnum = i;
			int j;
			fprintf(stderr, "VCPU %i starting, pid %i\n", my_procnum, my_pid);
			for(j = 0; j < moves; j++){
				sem_wait(shell + from[my_procnum]);
				sem_inc(shell + to[my_procnum]);
			}
			fprintf(stderr, "Child %i (pid %i) done, signal handler was invoked %i times\n", my_procnum, my_pid, get_handlered());
			fprintf(stderr, "VCPU %i done\n", my_procnum);
			exit(0);
		}
	}
	
	fprintf(stderr, "Main process spawed all children, waiting\n");


	for(i = 0; i < 6; i++){
		int wstatus;
		int temp_pid;
		if((temp_pid = wait(&wstatus)) == -1){
			fprintf(stderr, "Wait Error: %s", strerror(errno));
		}
		fprintf(stderr, "Child pid %i exited w/ %i\n", temp_pid, WEXITSTATUS(wstatus));
	}
	fprintf(stderr, "Sem #	val	Sleeps	Wakes\n");
	for(i = 0; i < 3; i++){
		fprintf(stderr, "%-10i%-10i\n", i, shell[i].count);
		int j;
		for(j = 0; j < 6; j++){
			fprintf(stderr, "VCPU %-4i         %9i %9i\n", j, shell[i].sleep[j], shell[i].woke[j]);

		}
	}





	return 0;
}
