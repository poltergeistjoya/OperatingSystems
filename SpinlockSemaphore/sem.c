#include <signal.h>
#include <stddef.h>
#include <unistd.h>

#include "sem.h"
#include "spinlock.h"

int handlered = 0;

static void handler(){ //dummy handler
	handlered++;
}

int get_handlered(){
	return handlered;
}

void sem_init(struct sem *s, int count){
	s->count = count;
	s->spinlock = 0;
	int i;
	for(i = 0; i<NUMPROC; i++){
		s->sleep[i] = 0;
		s->woke[i] = 0;
		s->waitlist[i] = -1;
	}

	signal(SIGUSR1, handler);
		
}

int sem_try(struct sem *s){
	spin_lock(&(s->spinlock));
	
	if(s->count > 0){
		s->count--;
		spin_unlock(&(s->spinlock));
		return 1;
	}
	else{
		spin_unlock(&(s->spinlock));
		return 0;
	}
}

void sem_wait(struct sem *s){
	while(1){
		spin_lock(&(s->spinlock));
		if(s->count > 0){
			s->count--;
			spin_unlock(&(s->spinlock));
			return;
		}
		sigset_t mask, oldmask;
		sigemptyset(&mask);
		sigemptyset(&oldmask);
		sigaddset(&mask, SIGUSR1);
		sigprocmask(SIG_BLOCK, &mask, &oldmask);

		s->waitlist[my_procnum] = getpid();

		spin_unlock(&(s->spinlock));
		s->sleep[my_procnum]++;
		sigsuspend(&oldmask);
	
		s->woke[my_procnum]++;
		sigprocmask(SIG_UNBLOCK, &mask, &oldmask);
		
		s->waitlist[my_procnum] = -1;
	}
}

void sem_inc(struct sem *s){
	spin_lock(&(s->spinlock));
	s->count++;
	if(s->count > 0){
		int i;
		for(i = 0; i < NUMPROC; i++){
			if(s->waitlist[i] != -1){
				kill(s->waitlist[i], SIGUSR1);
				s->waitlist[i] = -1;
			}
		}
	}
	spin_unlock(&(s->spinlock));
}
