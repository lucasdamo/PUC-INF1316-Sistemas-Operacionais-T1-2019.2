#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>




int main(void){
	int i;

	for(i=1; i < 12; i++){
		printf("P1 (pid = %d) Executando loop %d/11\n", getpid(), i);
		sleep(1);
	}
	
	return 0;
}