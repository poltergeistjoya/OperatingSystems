#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

//real time signals #>=32, as many signal deliveries as generated
//conv signals <32 only one instance of signal will be delivered
int i;
void h(int signum){
	//printf("\nInside handler function\n");
	i++;
}

int main(int argc, char *argv[]){
	int j = 0;
	pid_t pid = getpid();
	int forknum = atoi(argv[1]);
	int sigrepeat = atoi(argv[2]);
	int signum = atoi(argv[3]);
	struct sigaction sa;
		sa.sa_flags = SA_RESTART;
		sa.sa_handler = h;
		sigemptyset(&sa.sa_mask);
	if(sigaction(signum, &sa, NULL) < 0){
		fprintf(stderr, "Sigaction failed with error: %s\n", strerror(errno));
		exit(0);
	}
	for(j; j < forknum; j++){
		switch(fork()){
		case -1:
			perror("Fork failed\n"); exit(1);
			break;
		case 0:
			;
			int z = 0;	
			for(z; z < sigrepeat; z++){
			kill(pid,signum);
			}
			exit(0);
			break;
		default:
			wait(NULL);
			break;
		}
	}

	fprintf(stdout, "%d signals sent, %d signals received\n", forknum * sigrepeat, i);

	return 0; 
}
