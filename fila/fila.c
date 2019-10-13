/***************************************************************************
*  Módulo de implementação: Fila
*
*  Projeto: PUC Rio INF1316 Sistemas Operacionais T1 2019.2
*  Gestor:  LES/DI/PUC-Rio
*  Autores: Lucas Rebello Damo
*
*  Histórico de evolução:
*     Versão	Data		Observações
*     1			05/10/2019	Versão inicial
*
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include 'fila.h'
#define DEBUG 1


typedef struct no {
	pid_t processo;
	int prioridade;
	struct no * prox;
}

struct fila {
	No * primeiro;
	int contador;

}

Fila *pFila = NULL;

void inicializaFila(void){
	pFila = malloc(sizeof(Fila));
	pFila->primeiro = NULL;
	pFila->contador = 0;
}


void insereFila(int prioridade, pid_t pid){
	No * novo = (No *) malloc(sizeof(No));	
	novo->processo = pid;
	novo->prioridade = prioridade;
	if(pFila == NULL) inicializaFila();
	
	if(pFila->primeiro == NULL){
		pFila->primeiro = novo;
		novo->prox = NULL;
	}
	else{
		No *paux, *paux2;
		int flagMudou;
		paux = pFila->primeiro;
		paux2 = NULL;
		flagMudou = 0;
		while(paux != NULL){
			if(novo->prioridade < paux->prioridade){
				if(paux2 == NULL){
					novo->prox = pFila->primeiro;
					pFila->primeiro = novo;
					flagMudou = 1;
					break;
				}
				else {
					novo->prox = paux;
					paux2->prox = novo;
					flagMudou = 1;
				}
			}
			paux2 = paux;
			paux = paux->prox;
		}
		if(flagMudou == 0){
			novo->prox = NULL;
			paux2->prox = novo;
		}
	}
	pFila->contador = pFila->contador + 1;
}

pid_t retiraPrimeiro(void){
	No *primeiro;
	pid_t pid;
	if(pFila == NULL) return 0;
	if(pFila->primeiro == NULL) return 0;
	primeiro = pFila->primeiro;
	pid = primeiro->processo;
	pFila->primeiro = pFila->primeiro->prox;
	free(primeiro);
	pFila->contador--;
	return pid;
}

void esvaziaFila(void){
	free(pFila);
	pFila = NULL;
}

#ifdef DEBUG
int main(void){
	insereFila(10, 1);
	insereFila(20, 2);
	insereFila(30, 3);
	insereFila(25, 99);
	printf("Retira %d\n", retiraPrimeiro());
	printf("Retira %d\n", retiraPrimeiro());
	printf("Retira %d\n", retiraPrimeiro());
	printf("Retira %d\n", retiraPrimeiro());
	printf("Retira %d\n", retiraPrimeiro());
	insereFila(30, 3);
	insereFila(25, 99);
	insereFila(20, 2);
	insereFila(10, 1);
	printf("Retira %d\n", retiraPrimeiro());
	printf("Retira %d\n", retiraPrimeiro());
	printf("Retira %d\n", retiraPrimeiro());
	printf("Retira %d\n", retiraPrimeiro());
	printf("Retira %d\n", retiraPrimeiro());





}
#endif	

