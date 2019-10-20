#include <stdio.h>
#include <unistd.h>
/*cpu bound depende mais do processador do que da entrada ou saida*/
int main(int argc, char const* argv[]){
	int i = 0;
	float num;
	
	printf("01");
	while (1){
		num = 1 + 2 + 3;
		num = num/2;
		if (i > 200) {/*AUMENTAR O LOOP*/
			return 0;
		}
	}
	i++;
	return 0;
}