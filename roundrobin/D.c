#include <stdio.h>
#include <unistd.h>
#include <signal.h>
/*io bound manda sinal qnd i chega a 7*/
int main(int argc, char const* argv[]){
	int i = 0;
	
	printf("03");
	while (i < 15) {
		sleep(1);
		i++;
		if (i == 7) {
			kill(getppid(), SIGILL);
		}
	}
	return 0;
}