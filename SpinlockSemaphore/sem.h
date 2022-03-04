#ifndef SEM_H

#define NUMPROC 64
int my_procnum;

struct sem{
	char spinlock; 
	int count;  
	int sleep[NUMPROC];
	int woke[NUMPROC];
	int waitlist[NUMPROC];
};

int get_handlered();

void sem_init(struct sem *s, int count);

int sem_try(struct sem *s);

void sem_wait(struct sem *s);

void sem_inc(struct sem *s);


#define __SEM_H
#endif
