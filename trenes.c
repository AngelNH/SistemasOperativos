#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include "utils.h"
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/wait.h>


#define CICLOS 10
#define PROCESOS 3

void proceso(int i);


SEM * semaphor;
int * g;
int g0=0;
int g1=0;
char *pais[5]={"Peru","Bolvia","Colombia"};

// Macro que incluye el código de la instrucción máquina xchg
#define atomic_xchg(A,B) 	__asm__ __volatile__(	\
							"   lock xchg %1,%0 ;\n"	\
							: "=ir"  (A)			\
							: "m"  (B), "ir" (A)		\
							);

int main(){
	pid_t p = getpid();
	//memoria compartida de semaforo
	//reservar espacio de memoria para semaforo.
	int id = shmget(IPC_PRIVATE,sizeof(struct SEMAPHOR),IPC_CREAT | 0666);
	int id1 = shmget(IPC_PRIVATE,sizeof(2*sizeof(int)),IPC_CREAT | 0666);
	//hacer attached de la memoria con una direccion.
	semaphor = (struct SEMAPHOR *) shmat(id,NULL,0);
	g = (int*) shmat(id1,NULL,0);
	initsem(semaphor);
	
	//comienza 
	int i=0;
	int status;
	
	for(i=0;i<PROCESOS;i++){
		p= fork();
		if(p==0){
			proceso(i);
		}
	}
	
	//espera para termino de hilos
	for(i=0;i<PROCESOS;i++){
		wait(&status);
	}
	
	//liberar memoria compartida
	shmdt(semaphor);
}


void proceso(int i){
	//printf("Entra al proceso el hilo %d\n",i);
	int k;
	int l0;
	int l1;
	for(k=0;k<CICLOS;k++){
		l0=1;
		l1=1;
		// Llamada waitsem implementada en la parte 3
		//realizar el x
		//printf("Antes del xchg %d\n",i);
		do { atomic_xchg(l0,g[0]); } while(l0!=0);
		//printf("Antes del semaforo %d\n",i);
		waitsem(semaphor);
		printf("Entra %s ",pais[i]);
		fflush(stdout);
		sleep(rand()%3);
		printf("- %s Sale\n",pais[i]);
		// Llamada waitsignal implementada en la parte 3
		do { atomic_xchg(l1,g[1]); } while(l1!=0);
		signalsem(semaphor);
		// Espera aleatoria fuera de la sección crítica
		sleep(rand()%3);
		l0=1;
		l1=1;
		g[0]=0;
		g[1]=0;
	}
	exit(0);
	// Termina el proceso
}