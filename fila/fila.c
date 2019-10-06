/***************************************************************************
*  Módulo de implementação: Fila
*
*  Projeto: PUC Rio INF1316 Sistemas Operacionais T1 2019.2
*  Gestor:  LES/DI/PUC-Rio
*  Autores: Lucas Rebello Damo
*           Luiza Del Negro
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
#define DEBUG 1


typedef struct no {
	pid_t processo;
	int prioridade;
	struct no * prox;
} No;

typedef struct fila {
	No * primeiro;
	int contador;

} Fila;

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
		paux = pFila->primeiro;
		paux2 = NULL;
		while(paux != NULL){
			if(novo->prioridade < paux->prioridade){
				if(paux2 == NULL){
					pFila->primeiro = novo;
				}
				else {
					novo->prox = paux;
				}
			}
			paux2 = paux;
			paux = paux->prox;
		}
	}
	pFila->contador = pFila->contador + 1;
}

pid_t retiraPrimeiro(void){
	No *paux;
	pid_t primeiro;
	primeiro = pFila->primeiro->processo;
	paux = pFila->primeiro;
	pFila->primeiro = pFila->primeiro->prox;
	free(paux);

	return primeiro;


}

void esvaziaFila(void){
	free(pFila);
}

#ifdef DEBUG
int main(void){
	insereFila(10, 0);
	insereFila(20, 1);
	insereFila(1, 2);
	printf("Retira %d\n", retiraPrimeiro());
}
#endif	

