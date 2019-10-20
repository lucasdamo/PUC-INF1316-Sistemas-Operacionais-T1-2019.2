/***************************************************************************
*  STUB
*
*  Projeto: PUC Rio INF1316 Sistemas Operacionais T1 2019.2
*
*  Descrição: Stub para testar o escalonador
*
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_IMPRESSOES 10

int main (void){
	int i;
	pid_t pidProc = getpid();
	printf("%d INICIADO -------------\n", pidProc);
	for(i=1; i<=NUM_IMPRESSOES; i++){
		printf("%d impressao %d / %d\n", pidProc, i, NUM_IMPRESSOES);
		sleep(1);
	}
	return 0;
}