/***************************************************************************
*
*  Projeto: PUC Rio INF1316 Sistemas Operacionais T1 2019.2
*  Gestor:  LES/DI/PUC-Rio
*  Autores: Luiza Del Negro
*			Lucas Rebello Damo
*
*  Histórico de evolução:
*     Versão	Data		Observações
*     1			20/10/2019	Versão inicial utilizando os moldes de funções de fila de prioridades feitas por Lucas Damo
*
***************************************************************************/

#include "fila.h"
#define DEBUG 0
#define QUANTUM 1

union cmdpid {
	pid_t processo;
	char * comando;
};

struct no {
	Cmdpid * cp;
	int tipoCmdPid;
	int pid;
	char * comandodoprograma;
	int status;
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

static void insereFila( Cmdpid * cp, int tipocp){
	No *novo = (No *) malloc(sizeof(No));	
	novo->cp = cp;
	novo->tipoCmdPid = tipocp;
	novo->prox=NULL;
	novo->status = 0;
	novo->pid = 0;

	if(pFila == NULL) inicializaFila();
	
	if(pFila->primeiro == NULL){
		pFila->primeiro = novo;
		novo->prox = NULL;
	}
	else{/*Insere sempre no fim da fila*/
		No *paux, *paux2;
		paux = pFila->primeiro;
		paux2 = NULL;
		while(paux != NULL){
			paux2 = paux;
			paux = paux->prox;
		}
		novo->prox = NULL;
		paux2->prox = novo;	
	}
	pFila->contador = pFila->contador + 1;
}

void insereFilaCmd(char*comando){
	Cmdpid *cp;
	char * cmm;
	cp=(Cmdpid*)malloc(sizeof(Cmdpid));
	cmm=(char*)malloc(255*sizeof(char));
	strcpy(cmm, comando);
	cp->comando=cmm;
	insereFila(cp,0);
}

void insereFilaPid(pid_t pid){
	Cmdpid * cp;
	cp=(Cmdpid*)malloc(sizeof(Cmdpid));
	cp->processo=pid;
	insereFila(cp,1);
}




int retiraPrimeiro(char*comando,pid_t*pid){
	No*primeiro;
	Cmdpid* cp;
	int tipo;
	if(pFila==NULL) return -1;
	if(pFila->primeiro==NULL) return -1;
	primeiro=pFila->primeiro;
	cp=primeiro->cp;
	tipo=primeiro->tipoCmdPid;
	pFila->primeiro=pFila->primeiro->prox;
	free(primeiro);
	pFila->contador--;
	if(tipo==0){
		strcpy(comando, cp->comando);
		return 0;
	}
	else{
		*pid=cp->processo;
		return 1;
	}
}

void esvaziaFila(void){
	free(pFila);
	pFila = NULL;
}
