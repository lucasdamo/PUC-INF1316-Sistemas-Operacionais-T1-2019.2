/***************************************************************************
*  Módulo de implementação: Fila
*
*  Projeto: PUC Rio INF1316 Sistemas Operacionais T1 2019.2
*  Gestor:  LES/DI/PUC-Rio
*  Autores: Lucas Rebello Damo
*			Luiza Del Negro
*
*  Histórico de evolução:
*     Versão	Data		Observações
*     1			27/10/2019	Versão inicial
*
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include "fila.h"
#define DEBUG 0

union cmdpid {
	pid_t processo;
	char * comando;
};


struct no {
	Cmdpid * cp;
	int tipoCmdPid; // 0 Se for comando e 1 se for Pid
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

static void insereFila(Cmdpid * cp, int tipocp){
	No * novo = (No *) malloc(sizeof(No));	
	novo->cp = cp;
	novo->tipoCmdPid = tipocp;
	novo->prox = NULL;
	if(pFila == NULL) inicializaFila();
	
	if(pFila->primeiro == NULL){
		pFila->primeiro = novo;
	}
	else{
		No *paux, *paux2;
		paux = pFila->primeiro;
		while(paux->prox != NULL){
			paux = paux->prox;
		}
		paux->prox = novo;
	}

	pFila->contador = pFila->contador + 1;
}

void insereFilaCmd(char * comando){
	Cmdpid * cp;
	char * cmm;
	cp = (Cmdpid *)malloc(sizeof(Cmdpid));
	cmm = (char *)malloc(255 * sizeof(char));
	strcpy(cmm, comando);
	cp->comando = cmm;
	insereFila(cp, 0);
}
void insereFilaPid(pid_t pid){
	Cmdpid * cp;
	cp = (Cmdpid *)malloc(sizeof(Cmdpid));
	cp->processo = pid;
	insereFila(cp, 1);
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
	int a;
	insereFilaPid(1);
	insereFilaPid(2);
	insereFilaPid(3);
	insereFilaPid(99);
	a = retiraPrimeiro(bufferComando, &bufferPid);
	printf("%d Retira %d\n", a, bufferPid);
	a = retiraPrimeiro(bufferComando, &bufferPid);
	printf("%d Retira %d\n", a, bufferPid);
	a = retiraPrimeiro(bufferComando, &bufferPid);
	printf("%d Retira %d\n", a, bufferPid);
	a = retiraPrimeiro(bufferComando, &bufferPid);
	printf("%d Retira %d\n", a, bufferPid);
	a = retiraPrimeiro(bufferComando, &bufferPid);
	printf("%d Retira %d contador %d\n", a, bufferPid, pFila->contador);
	insereFilaPid(3);
	insereFilaPid(99);
	insereFilaPid(2);
	insereFilaPid(1);
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
	insereFilaCmd("Testando comando1");
	insereFilaCmd("Testando comando2");
	insereFilaCmd("Testando comando3");
	insereFilaCmd("Testando comando4");
	retiraPrimeiro(bufferComando, &bufferPid);
	printf("Retira %s\n", bufferComando);
	retiraPrimeiro(bufferComando, &bufferPid);
	printf("Retira %s\n", bufferComando);
	retiraPrimeiro(bufferComando, &bufferPid);
	printf("Retira %s\n", bufferComando);
	retiraPrimeiro(bufferComando, &bufferPid);
	printf("Retira %s\n", bufferComando);
	return 0;
}
#endif	

