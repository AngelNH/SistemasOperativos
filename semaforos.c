#include<stdio.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include"semaphores.h"
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/wait.h>


#define CICLOS 10
#define PROCESOS 3

#define atomic_xchg(A,B) 	__asm__ __volatile__(	\
							"   lock xchg %1,%0 ;\n"	\
							: "=ir"  (A)			\
							: "m"  (B), "ir" (A)		\
							);


void proceso (int i);
int * g;
char *pais[5]={"Peru","Bolvia","Colombia"};

SEM_ID * exmut_sem;

int main(){
	int p=getpid();
	//Reservar memoria compartida.
	int id = shmget(IPC_PRIVATE,sizeof(int),IPC_CREAT | 0666);
	int id1 = shmget(IPC_PRIVATE,sizeof(2*sizeof(int)),IPC_CREAT | 0666);
	//hacer attached de la memoria con una direccion.
	exmut_sem = (int *) shmat(id,NULL,0);
	g = (int*) shmat(id1,NULL,0);
	//
	
	*exmut_sem=createsem(0x1234,1);//check 
	
	//Creación de procesos
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
	//eliminar el semaforo
	erasesem(*exmut_sem);
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
		semwait(*exmut_sem);
		printf("Entra %s ",pais[i]);
		fflush(stdout);
		sleep(rand()%3);
		printf("- %s Sale\n",pais[i]);
		// Llamada waitsignal implementada en la parte 3
		do { atomic_xchg(l1,g[1]); } while(l1!=0);
		semsignal(*exmut_sem);
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