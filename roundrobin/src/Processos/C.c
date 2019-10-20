#include <stdio.h>
#include <unistd.h>
#include <signal.h>
/*io bound manda sinal qnd i=3*/
int main(int argc, char const* argv[]){
	int i = 0;
	
	printf("02");
	while (i < 15) {
		sleep(1);
		i++;
		if (i == 3) {
			kill(getppid(), SIGILL);
		}
	}
	return 0;
}