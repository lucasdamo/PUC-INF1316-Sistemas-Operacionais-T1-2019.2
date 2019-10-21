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
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>


#include "fila.h"
#define DEBUG 0
#define QUANTUM 1


struct no {
	int pid;
	char * nomedoprograma;
	char * comandodoprograma;
	int status;
	struct no * prox;
};
struct fila {
	struct no * primeiro;
	int contador;
};


Fila *pFila;

/** Ponteiro para funcao de handler
de sinais */
int * waitingMemory;
int PROCESSOS_FINALIZADOS = 0;

void inicializaFila(void){
	pFila = malloc(sizeof(Fila));
	pFila->primeiro = NULL;
	pFila->contador = 0;
}

static void insereFila( char * comando_programa,char * nome_programa){
	No * novo = (No *) malloc(sizeof(No));	

	novo->nomedoprograma=(char*) malloc(sizeof(char) * strlen(nome_programa));
	strcpy(novo->nomedoprograma,nome_programa);
	novo->comandodoprograma=(char*) malloc(sizeof(char) * strlen(comando_programa));
	strcpy(novo->comandodoprograma,comando_programa);
	novo->prox=NULL;
	novo->status=0;
	novo->pid=0;

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
			paux2 = paux;
			paux = paux->prox;
		}
		novo->prox = NULL;
		paux2->prox = novo;
		
	}
	pFila->contador = pFila->contador + 1;
}




// int retiraPrimeiro(char * comando, pid_t * pid, int * prioridade){
// 	No *primeiro;
// 	Cmdpid * cp;
// 	int tipo;
// 	if(pFila == NULL) return -1;
// 	if(pFila->primeiro == NULL) return -1;
// 	primeiro = pFila->primeiro;
// 	pFila->primeiro = pFila->primeiro->prox;
// 	free(primeiro);
// 	pFila->contador--;
// 	if(tipo == 0){
// 		strcpy(comando, cp->comando);
// 		return 0;
// 	}
// 	else{
// 		*pid = cp->processo;
// 		return 1;
// 	}
// }

void esvaziaFila(void){
	free(pFila);
	pFila = NULL;
}

void MostraFila(void){
	No* aux=pFila->primeiro;
	while(aux!=0){
		printf("comanando: %s programa: %s  pid: %d\n",aux->comandodoprograma, aux->nomedoprograma,aux->pid);
		aux=aux->prox;
	}

}

void InsereFim(No * no){
	no->status=0;
	if(pFila->primeiro == NULL){
		pFila->primeiro = no;
		no->prox = NULL;
	}
	else{
		No *paux, *paux2;
		paux = pFila->primeiro;
		paux2 = NULL;
		while(paux != NULL){
			paux2 = paux;
			paux = paux->prox;
		}
		no->prox = NULL;
		paux2->prox = no;	
	}
}

void AnalisaStatus(void){

}

/*enquanto os processos nao acabam, eu vou por toda a fila*/
void RoundRobin(void){
	No*aux=pFila->primeiro;

	int i=0, j=0;
	/*tem que ver o status dos processos em espera antes?*/
	AnalisaStatus();
	while(PROCESSOS_FINALIZADOS < pFila->contador){
		if(aux->pid==0){/* Se o pid for zero ele nao foi inicializado, é a primeira vez que ele vai rodar*/
			aux->pid=fork();
			if(aux->pid!=0){/*é o pai*/
				sleep(QUANTUM);/* dorme enquanot o flho executa*/
				printf("interrompendo o processo %s \n", aux->nomedoprograma);
				kill(aux->pid, SIGSTOP);
				/*interrompe e manda pro fim da fila*/
				aux->status=2;
				No*aux2=aux;
				aux=aux->prox;
				InsereFim(aux2);
				free(aux2);

			}
			else{
				printf("Executando o processo %s \n", aux->nomedoprograma);
				execve(aux->nomedoprograma, NULL, NULL);
			}
		}
		else{/*se ja foi inicializado*/
			if(aux->status!=3){/* Se ainda nao acabou*/
				int status;
				int wpid = waitpid(aux->pid, &status, WNOHANG);
				printf("Executando o processo %s \n", aux->nomedoprograma);
				kill(aux->pid, SIGCONT);
				sleep(QUANTUM);
				printf("interrompendo o processo %s \n", aux->nomedoprograma);
				kill(aux->pid, SIGSTOP);
				if(wpid && (aux->status != 3) && WIFEXITED(status) && (WEXITSTATUS(status) == 0)){/*Se nao acabou, tem que ver o status dele de novo */
					printf("Processo %s finalizado \n", aux->nomedoprograma);
					aux->status = 3;
					kill(aux->pid, SIGKILL);
					/*aqui coloca func que remove o elemento da lista*/
					/*interrompe e manda pro fim da fila*/
					No*aux2=aux;
					aux=aux->prox;
					free(aux2);
					PROCESSOS_FINALIZADOS=PROCESSOS_FINALIZADOS+1;
				}
				/*se nao acabou, tira do fim e passa pro final*/
				/*interrompe e manda pro fim da fila*/
				aux->status=2;
				No*aux2=aux;
				aux=aux->prox;
				InsereFim(aux2);
				free(aux2);
			}
		}
		aux=aux->prox;
	}

}

#ifdef DEBUG
int main(void){
	char comando[4];
	char programa[50];
	pid_t pid;
	FILE* arq = fopen("/Users/luiza.fernandes/Luiza/Projects/PUC-INF1316-Sistemas-Operacionais-T1-2019.2/roundrobin/entrada_rr.txt", "r");

	if(arq == NULL){
		printf("ERRO AO ABRIR O ARQUIVO");
		exit(1);
	}
	
	while(fscanf(arq, "%s %s ", comando, programa) == 2){
		insereFila(comando,programa);
	}
	MostraFila();
	
	RoundRobin();

	
	fclose(arq);

	return 0;
}
#endif	

