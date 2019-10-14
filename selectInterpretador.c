#include <stdio.h>
#include <stdlib.h>



int main(int argc, char *argv[]){
	switch(argc){
		case 2:
			printf("ROUND ROBIN\n");
			break;
		case 3:
			printf("Prioridade\n");
			break;
		case 4:
			printf("Rea time\n");
			break;
		default:
			printf("Comando nao reconhecido\n\
					Run <nome_programa> PR=<número de 0 a 7>, para escalonamento PROPRIDADE\n\
					Run <nome_programa>, para o ROUND-ROBIN\n\
					Run <nome_programa> I=<momento-início> D=<tempo-duração>, para REAL-TIME\n");
			break;
	}
}