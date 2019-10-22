/***************************************************************************
*  Módulo de implementação: RoundRobin
*
*  Projeto: PUC Rio INF1316 Sistemas Operacionais T1 2019.2
*  Gestor:  LES/DI/PUC-Rio
*  Autores: Luiza Del Negro
*
*  Histórico de evolução:
*     Versão	Data		Observações
*     1			20/10/2019	Versão inicial utilizando os moldes de funções de fila de prioridades feitas por Lucas Damo
*
***************************************************************************/

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

int PROCESSOS_FINALIZADOS = 0;

void inicializaFila(void){
	pFila = malloc(sizeof(Fila));
	pFila->primeiro = NULL;
	pFila->contador = 0;
}

static void insereFila( char * comando_programa,char * nome_programa){
	No *novo = (No *) malloc(sizeof(No));	

	novo->nomedoprograma = (char*) malloc(sizeof(char) * strlen(nome_programa));
	strcpy(novo->nomedoprograma,nome_programa);
	novo->comandodoprograma = (char*) malloc(sizeof(char) * strlen(comando_programa));
	strcpy(novo->comandodoprograma,comando_programa);
	novo->prox=NULL;
	novo->status = 0;
	novo->pid = 0;

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



void esvaziaFila(void){
	free(pFila);
	pFila = NULL;
}

void MostraFila(void){
	No* aux = pFila->primeiro;
	while(aux!=0){
		printf("comanando: %s programa: %s  \n",aux->comandodoprograma, aux->nomedoprograma);
		aux = aux->prox;
	}
}

void Dequeue(void){
	printf("DEQUEUE- TIRANDO %s na fila\n", pFila->primeiro->nomedoprograma);
	No *aux;
	No *aux2;
	aux = pFila->primeiro;// que é o que vamos tirar
	if(aux->prox==NULL){/*Fila vazia*/
		printf("DEQUEUE- A FILA FICA VAZIA \n");
		aux = NULL;
		pFila->primeiro = NULL;
	}
	aux2 = aux->prox;// o que sera o proximo primeiro
	pFila->primeiro = aux2;// trocando para o proximo da fila
	printf("DEQUEUE - ATUAL PRIMEIRO %s \n",pFila->primeiro->nomedoprograma);
}

void Enqueue(No * no){
	No *novo = (No *) malloc(sizeof(No));	
	No *paux;
	paux = pFila->primeiro;
	printf("ENQUEUE-Botando %s na fila\n", no->nomedoprograma);
	novo = no;
	novo->status = 0;
	if(no==NULL){
		printf("ENQUEUE-Nao tem mais nada para adicionar\n");
		return;
	}
	if(pFila->primeiro == NULL){
		pFila->primeiro = novo;
		novo->prox = NULL;
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
	int i=0, j=0;
	printf("00\n");
	for(i=0;i<pFila->contador;i++){
		aux->pid = fork();
		if(aux->pid==0){/*É filho*/
	/*CHAMANDO NA PRIMEIRA VEZ PARA TODOS E QUANDO ISSO TERMINAR O */
		printf("primeiro filho-Executando o processo %s \n", aux->nomedoprograma);
		execv(aux->nomedoprograma, NULL);/**/
		printf("primeiro filho-interrompendo o processo %s \n", aux->nomedoprograma);	
		exit(1);		
		}
		if(aux->pid>0){/*PAI controla filho*/
			sleep(QUANTUM);/*Pai tem que esperar o filho*/
			/* antes de acabar tem que ver se acabou*/
			int status;
			int wpid = waitpid(aux->pid, &status, WNOHANG);/*analisa o status do filho*/
			if(wpid && WIFEXITED(status) && (WEXITSTATUS(status) == 0)){/*SE O FILHO ACABOU, ACABOU TBM*/
				printf("FIM-Processo %s finalizado \n", aux->nomedoprograma);
				kill(aux->pid, SIGKILL);// termina o processo
				Dequeue();// tira da fila					printf("FIM-Dentro dos finalizados, passa para o proximo = %s\n", aux->nomedoprograma);
				PROCESSOS_FINALIZADOS=PROCESSOS_FINALIZADOS+1;
			}
			kill(aux->pid, SIGSTOP);// para o processo
			/*se nao acabou, tira e passa pro final*/
			printf("primeiro pai-Colocando: %s no fim da fila \n", aux->nomedoprograma);
			No * aux2 = (No *) malloc(sizeof(No));	
			aux2= pFila->primeiro;// que é o que vamos tirar
			aux=aux->prox;
			Enqueue(aux2);
			printf("primeiro pai-Proximo elemento: %s \n", aux->nomedoprograma);
			
			kill(aux->pid, SIGCONT);
			
		}
		
		if(aux==NULL){
			printf("ACABOU!\n");
		}
		if(aux->pid<0){/*ERRO NA ALOCACAO*/
		printf("Erro na alocação");
		exit(1);
		}
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
	/*Cria a fila com os processos do arquivo*/
	while(fscanf(arq, "%s %s ", comando, programa) == 2){
		insereFila(comando,programa);
	 }
	// MostraFila();
	
	RoundRobin();

	
	fclose(arq);

	return 0;
}
#endif	

