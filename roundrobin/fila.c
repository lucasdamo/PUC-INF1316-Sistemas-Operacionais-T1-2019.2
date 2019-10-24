/***************************************************************************
*  Módulo de implementação: RoundRobin
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

int PROCESSOS_FINALIZADOS = 0;

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

void MostraFila(void){
	No* aux = pFila->primeiro;
	while(aux!=0){
		printf("comanando: %s \n",aux->comandodoprograma);
		aux = aux->prox;
	}
}

void Dequeue(void){
	
	No *aux;
	No *aux2;
	aux = pFila->primeiro;// que é o que vamos tirar

	printf("DEQUEUE-Tirando %s da fila\n", aux->comandodoprograma);
	if(aux->prox==NULL){/*Fila vazia*/
		printf("DEQUEUE- A FILA FICA VAZIA \n");
		free(aux);
		pFila->primeiro = NULL;
	}
	else{
		aux2 = aux->prox;// o que sera o proximo primeiro
		pFila->primeiro = aux2;// trocando para o proximo da fila
		free(aux);
	}
}
void Enqueue(No * no){
	No *paux;
	paux = pFila->primeiro;
	printf("ENQUEUE-Botando %s na fila\n", no->comandodoprograma);
	if(no==NULL){
		printf("ENQUEUE-Nao tem mais nada para adicionar\n");
		return;
	}
	if(pFila->primeiro == NULL){
		pFila->primeiro = no;
		no->prox = NULL;
	}
	else{
		while(paux->prox!= NULL){
			paux = paux->prox;
		}
		paux->prox=no;	
	}
}



/*enquanto os processos nao acabam, eu vou por toda a fila*/
void RoundRobin(void){
	No *aux = pFila->primeiro;
	int i=0;
	printf("00\n");
	for(i=0;i<pFila->contador;i++){/*Inicia todos os elementos do vetor de processos*/
		aux->pid = fork();
		if(aux->pid==0){/*É filho*/
			printf("primeiro filho-Executando o processo %s \n", aux->comandodoprograma);
			execv(aux->comandodoprograma, NULL);
			exit(1);		
		}
		if(aux->pid>0){/*PAI controla filho*/
			sleep(QUANTUM);/*Pai tem que esperar o filho*/
			/* antes de acabar tem que ver se acabou*/
			int status;
			int wpid = waitpid(aux->pid, &status, WNOHANG);/*analisa o status do filho*/
			if(wpid && WIFEXITED(status) && (WEXITSTATUS(status) == 0)){/*SE O FILHO ACABOU, ACABOU TBM*/
				printf("FIM-Processo %s finalizado \n", aux->comandodoprograma);
				//kill(aux->pid, SIGKILL);// termina o processo
				Dequeue();// tira da fila					printf("FIM-Dentro dos finalizados, passa para o proximo = %s\n", aux->nomedoprograma);
				PROCESSOS_FINALIZADOS=PROCESSOS_FINALIZADOS+1;
			}
			//else
			else{
				printf("primeiro filho-interrompendo o processo %s \n", aux->comandodoprograma);	
				kill(aux->pid, SIGSTOP);// para o processo
			/*se nao acabou, tira e passa pro final*/
				printf("pai-Colocando: %s no fim da fila \n", aux->comandodoprograma);
				No * aux2 = (No *) malloc(sizeof(No));	
				// RETIRA O PRIMEIRO RETIRAR O NO
				// APONTAR PARA O NOVO
				* aux2= (*pFila->primeiro);// que é o que vamos tirar
				Dequeue();
				Enqueue(aux2);
				printf("pai-Proximo elemento: %s \n", aux->comandodoprograma);
			}
		}
		if(aux==NULL){
			printf("ACABOU!\n");
		}
		if(aux->pid<0){/*ERRO NA ALOCACAO*/
			printf("Erro na alocação");
			exit(1);
		}
	}
	while(PROCESSOS_FINALIZADOS<pFila->contador){
		MostraFila();
		kill(aux->pid, SIGCONT);
		sleep(QUANTUM);
		int status;
		int wpid = waitpid(aux->pid, &status, WNOHANG);/*analisa o status do filho*/
		if(wpid && WIFEXITED(status) && (WEXITSTATUS(status) == 0)){/*SE O FILHO ACABOU, ACABOU TBM*/
			printf("FIM-Processo %s finalizado \n", aux->comandodoprograma);
			//kill(aux->pid, SIGKILL);// termina o processo
			Dequeue();// tira da fila					
			printf("FIM-Dentro dos finalizados, passa para o proximo = %s\n", aux->comandodoprograma);
			PROCESSOS_FINALIZADOS=PROCESSOS_FINALIZADOS+1;
		}
		else{
			printf("interrompendo o processo %s \n", aux->comandodoprograma);	
			kill(aux->pid, SIGSTOP);// para o processo
			/*se nao acabou, tira e passa pro final*/
			printf("Colocando: %s no fim da fila \n", aux->comandodoprograma);
			No * aux2 = (No *) malloc(sizeof(No));	
			// RETIRA O PRIMEIRO RETIRAR O NO
			// APONTAR PARA O NOVO
			* aux2= (*pFila->primeiro);// que é o que vamos tirar
			Dequeue();
			Enqueue(aux2);
			printf("Proximo elemento: %s \n", aux->comandodoprograma);
		}


	}

}

#ifdef DEBUG
int main(void){

	
	return 0;
}
#endif	

