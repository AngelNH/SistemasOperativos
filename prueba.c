#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include<signal.h>
#include "utils.h"

void algo(int  s);
int * semaphor;

int main(){
	
	
	*semaphor =2;
	//printf("s  = %d\n",*semaphor);
	algo(*semaphor);
	//printf("s  = %d\n",*semaphor);
}

void algo(int s){
	printf("s dentro de algo = %d\n",s);
	s ++;
	printf("s dentri de algo= %d\n",s);
}