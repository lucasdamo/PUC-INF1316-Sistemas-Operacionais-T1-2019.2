#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>




int main(void){
	int i;

	for(i=1; i < 5; i++){
		printf("P2 (pid = %d) Executando loop %d/4\n", getpid(), i);
		sleep(2);
	}
	
	return 0;
}