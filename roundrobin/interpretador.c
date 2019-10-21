#include <stdio.h>
#include <stdlib.h>
#include "roundrobin.h"



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
	}
}
