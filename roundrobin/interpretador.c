#include <stdio.h>
#include <stdlib.h>



int main(int argc, char* argv[]) {
	char* rr[] = { "roundrobin", NULL };

	int escalonador;

	printf("\n3 - Prioridade com Preemp��o\n2 - Round-Robin\n");
	scanf("%d", &escalonador);

	switch (escalonador) {
	case 2:
		printf("ROUND ROBIN\n");
		execve("/roundrobin", rr);
		break;
	case 3:
		printf("Prioridade\n");
		break;
	case 4:
		printf("Rea time\n");
		break;
	default:
		printf("Comando nao reconhecido\n\
					Run <nome_programa> PR=<n�mero de 0 a 7>, para escalonamento PROPRIDADE\n\
					Run <nome_programa>, para o ROUND-ROBIN\n\
					Run <nome_programa> I=<momento-in�cio> D=<tempo-dura��o>, para REAL-TIME\n");
		break;
	}
}