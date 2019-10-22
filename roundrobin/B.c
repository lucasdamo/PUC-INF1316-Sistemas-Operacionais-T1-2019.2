#include <stdio.h>
#include <unistd.h>
/*cpu bound depende mais do processador do que da entrada ou saida*/
int main(int argc, char const* argv[]){
	int i = 0;
	float num;
	
	printf("PROGRAMA B RODANDO");
	// while (1){
	// 	num = 1 + 2 + 3;
	// 	num = num/2;
	// 	if (i > 20000) {/*AUMENTAR O LOOP*/
	// 		return 0;
	// 	}
	// }
	// i++;
	for (int i = 0; i < 10; ++i){
		sleep(2);
	}
	return 0;
}