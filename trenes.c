#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include<signal.h>
#include "utils.h"

#define CICLOS 10


SEM * semaphor;


int main(){
	pid_t p = getpid();
	
	
	//memoria compartida de semaforo
	//reservar espacio de memoria para semaforo.
	int id = shmget(IPC_PRIVATE,sizeof(struct SEMAPHOR),IPC_CREATE | 0666);
	//hacer attached de la memoria con una direccion.
	semaphor = (struct SEMAPHOR *) shmat(id,NULL,0);
	
	
	
}


void proceso(int i){
	int k;
	for(k=0;k<CICLOS;k++){
		// Llamada waitsem implementada en la parte 3
		waitsem(semaphor);
		printf("Entra %s ",pais[i]);
		fflush(stdout);
		sleep(rand()%3);
		printf("- %s Sale\n",pais[i]);
		// Llamada waitsignal implementada en la parte 3
		signalsem(semaphor);
		// Espera aleatoria fuera de la sección crítica
		sleep(rand()%3);
	}
	exit(0);
	// Termina el proceso
}