
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define CICLOS 10
#define atomic_xchg(A,B) 	__asm__ __volatile__(	\
							"   lock xchg %1,%0 ;\n"	\
							: "=ir"  (A)			\
							: "m"  (B), "ir" (A)		\
							);

struct msg_buf{
	long tipo;
	int contenido;
};
int msgqid;
char *pais[3]={"Peru","Bolvia","Colombia"};
int *g;
struct msg_buf * x; //TODO hacer esto compartida.
void proceso(int i);

int main(){
	int pid;
	int status;
	int args[3];
	int i;
	srand(getpid());
	
	//Reservar memoria compartida.
	int id = shmget(IPC_PRIVATE,sizeof(struct msg_buf),IPC_CREAT | 0666);
	int id1 = shmget(IPC_PRIVATE,sizeof(2*sizeof(int)),IPC_CREAT | 0666);
	//hacer attached de la memoria con una direccion.
	x = (struct msg_buf *) shmat(id,NULL,0);
	g = (int*) shmat(id1,NULL,0);
	
	msgqid = msgget(0x1234,IPC_CREAT | 0666);
	x->contenido=1;
	msgsnd(msgqid,&x,sizeof(struct msg_buf),0);
	
	for(i=0;i<3;i++){
		// Crea un nuevo proceso hijo que ejecuta la función proceso()
		pid=fork();
		if(pid==0)
		proceso(i);
	}
	for(i=0;i<3;i++)
		pid = wait(&status);
	
	msgctl(msgqid,IPC_RMID,NULL);
}

void proceso(int i){
	int k;
	int l0;
	int l1;
	for(k=0;k<CICLOS;k++){
		do { atomic_xchg(l0,g[0]); } while(l0!=0);	
		do{
		msgrcv(msgqid,&x,sizeof(struct msg_buf),0,0);
		}while(x->contenido!= 1);
		x->contenido=0;
		msgsnd(msgqid,&x,sizeof(struct msg_buf),0);
		
		// Entrada a la sección crítica
		printf("Entra %s",pais[i]);
		fflush(stdout);
		sleep(rand()%3);
		printf("- %s Sale\n",pais[i]);
		// Salida de la sección crítica
		
		do { atomic_xchg(l1,g[1]); } while(l1!=0);
		x->contenido=1;
		msgsnd(msgqid,&x,sizeof(struct msg_buf),0);
		
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