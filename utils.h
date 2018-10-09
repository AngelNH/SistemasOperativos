#include<sys/types.h>
#include <stdio.h>
#include <stdlib.h>

#define MAXTHREAD 3

typedef struct _QUEUE{
	pid_t elements[MAXTHREAD];
	pid_t head;
	pid_t tail;
}QUEUE;

void _initqueue(QUEUE *q){
	q->head = 0;
	q->tail = 0;
}
void _enqueue(QUEUE *q,pid_t val){
	q->elements[q->head] = val;
	q->head++;
	q->head=q->head%MAXTHREAD;
}
pid_t _dequeue(QUEUE *q){
	pid_t valret;
	valret = q->elements[q->tail];
	q->tail++;
	q->tail = q->tail%MAXTHREAD;
	
	return(valret);
}



typedef struct SEMAPHOR{
	int count;
	QUEUE blocked;
	
}SEM;

void initsem (SEM * s){
	s->count=1;
	_initqueue(&(s->blocked));
}
void waitsem (SEM * s){
	s->count--;
	if(s->count <0){
		_enqueue(&s->blocked,getpid());
		kill(getpid(),SIGSTOP);
	}
}

void signalsem (SEM * s){
	s->count++;
	if(s->count <= 0){
		pid_t next = _dequeue(&s->blocked);
		kill(next,SIGCONT);
	}
}