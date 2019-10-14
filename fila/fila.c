/***************************************************************************
*  Módulo de implementação: Fila
*
*  Projeto: PUC Rio INF1316 Sistemas Operacionais T1 2019.2
*  Gestor:  LES/DI/PUC-Rio
*  Autores: Lucas Rebello Damo
*
*  Histórico de evolução:
*     Versão	Data		Observações
*	  2			13/10/2019	Adicionado suporte a comandos além de PID
*     1			05/10/2019	Versão inicial
*
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include "fila.h"
#define DEBUG 1

union cmdpid {
	pid_t processo;
	char * comando;
};


struct no {
	Cmdpid * cp;
	int tipoCmdPid; // 0 Se for comando e 1 se for Pid
	int prioridade;
	struct no * prox;
};
struct fila {
	struct no * primeiro;
	int contador;
};


Fila *pFila;

void inicializaFila(void){
	pFila = malloc(sizeof(Fila));
	pFila->primeiro = NULL;
	pFila->contador = 0;
}

static void insereFila(int prioridade, Cmdpid * cp, int tipocp){
	No * novo = (No *) malloc(sizeof(No));	
	novo->cp = cp;
	novo->tipoCmdPid = tipocp;
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

void insereFilaCmd(int prioridade, char * comando){
	Cmdpid * cp;
	char * cmm;
	cp = (Cmdpid *)malloc(sizeof(Cmdpid));
	cmm = (char *)malloc(255 * sizeof(char));
	strcpy(cmm, comando);
	cp->comando = cmm;
	insereFila(prioridade, cp, 0);
}
void insereFilaPid(int prioridade, pid_t pid){
	Cmdpid * cp;
	cp = (Cmdpid *)malloc(sizeof(Cmdpid));
	cp->processo = pid;
	insereFila(prioridade, cp, 1);
}


int retiraPrimeiro(char * comando, pid_t * pid){
	No *primeiro;
	Cmdpid * cp;
	int tipo;
	if(pFila == NULL) return -1;
	if(pFila->primeiro == NULL) return -1;
	primeiro = pFila->primeiro;
	cp = primeiro->cp;
	tipo = primeiro->tipoCmdPid;
	pFila->primeiro = pFila->primeiro->prox;
	free(primeiro);
	pFila->contador--;
	if(tipo == 0){
		strcpy(comando, cp->comando);
		return 0;
	}
	else{
		*pid = cp->processo;
		return 1;
	}
}

void esvaziaFila(void){
	free(pFila);
	pFila = NULL;
}

#ifdef DEBUG
int main(void){
	char bufferComando[255];
	pid_t bufferPid;
	insereFilaPid(10, 1);
	insereFilaPid(20, 2);
	insereFilaPid(30, 3);
	insereFilaPid(25, 99);
	retiraPrimeiro(bufferComando, &bufferPid);
	printf("Retira %d\n", bufferPid);
	retiraPrimeiro(bufferComando, &bufferPid);
	printf("Retira %d\n", bufferPid);
	retiraPrimeiro(bufferComando, &bufferPid);
	printf("Retira %d\n", bufferPid);
	retiraPrimeiro(bufferComando, &bufferPid);
	printf("Retira %d\n", bufferPid);
	retiraPrimeiro(bufferComando, &bufferPid);
	printf("Retira %d\n", bufferPid);
	insereFilaPid(30, 3);
	insereFilaPid(25, 99);
	insereFilaPid(20, 2);
	insereFilaPid(10, 1);
	retiraPrimeiro(bufferComando, &bufferPid);
	printf("Retira %d\n", bufferPid);
	retiraPrimeiro(bufferComando, &bufferPid);
	printf("Retira %d\n", bufferPid);
	retiraPrimeiro(bufferComando, &bufferPid);
	printf("Retira %d\n", bufferPid);
	retiraPrimeiro(bufferComando, &bufferPid);
	printf("Retira %d\n", bufferPid);
	retiraPrimeiro(bufferComando, &bufferPid);
	printf("Retira %d\n", bufferPid);
	insereFilaCmd(10, "Testando comando1");
	insereFilaCmd(30, "Testando comando2");
	insereFilaCmd(25, "Testando comando3");
	insereFilaCmd(20, "Testando comando4");
	retiraPrimeiro(bufferComando, &bufferPid);
	printf("Retira %s\n", bufferComando);
	retiraPrimeiro(bufferComando, &bufferPid);
	printf("Retira %s\n", bufferComando);
	retiraPrimeiro(bufferComando, &bufferPid);
	printf("Retira %s\n", bufferComando);
	retiraPrimeiro(bufferComando, &bufferPid);
	printf("Retira %s\n", bufferComando);






}
#endif	

