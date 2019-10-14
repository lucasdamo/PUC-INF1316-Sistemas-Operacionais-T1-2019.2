#include <stdio.h>
#include <unistd.h>
/*cpu bount mas com um loop menor*/
int main(int argc, char const* argv[]){
	int i = 0;
	printf("00");
	for (int i = 0; i < 10; ++i){
		sleep(1);
	}
	return 0;
}