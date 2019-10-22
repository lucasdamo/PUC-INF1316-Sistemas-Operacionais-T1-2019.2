/***************************************************************************
*  Módulo de interface: RoundRobin
*
*  Projeto: PUC Rio INF1316 Sistemas Operacionais T1 2019.2
*  Gestor:  LES/DI/PUC-Rio
*  Autores: Luiza Del Negro
*
*  Histórico de evolução:
*     Versãgio	Data		Observações
*     1			20/10/2019	Versão inicial utilizando os moldes de funções de fila de prioridades feitas por Lucas Damo
*
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>

typedef struct no No;
typedef struct fila Fila;

/***********************************************************************
*   Função: inicializaFila
*
*	Descrição: Inicializa a estrutura fila
*
*	Assertivas De Entrada:	
*		- A fila ainda não foi inicializada
*
*	Assertivas De Saída:	
*		- A fila foi recém inicializada e não possui nós
*
***********************************************************************/
void inicializaFila(void);

/***********************************************************************
*   Função: insereFila
*
*	Descrição: Adiciona elemento a fila
*
*	Assertivas De Entrada:	
*		- a fila, o comando do programa e o nome do programa a ser executado
*
*	Assertivas De Saída:	
*		- A fila com novos nós
*
***********************************************************************/
static void insereFila( char * comando_programa,char * nome_programa);

/***********************************************************************
*
*   Função: esvaziaFila
*
*	Descrição: Libera o espaço associado a fila, e reseta o ponteiro
*	para NULL.
*
*	Assertivas De Entrada:	
*		- A fila já foi inicializada
*
*	Assertivas De Saída:	
*		- A fila não existe mais
*
***********************************************************************/
void esvaziaFila(void);

/***********************************************************************
*   Função: MostraFila
*
*	Descrição: Mostra os componentes da fila na ordem em que sao apresentados
*
*	Assertivas De Entrada:	
*		- A fila existente
*
*
***********************************************************************/
void MostraFila(void);


/***********************************************************************
*   Função: Dequeue
*
*	Descrição: Tira primeiro elemento da fila
*
*	Assertivas De Entrada:	
*		- a fila
*
*	Assertivas De Saída:	
*		- A fila sem o nó inicial
*
***********************************************************************/
void Dequeue(void);

/***********************************************************************
*   Função: Enqueue
*
*	Descrição: Insere elemento no fim da fila
*
*	Assertivas De Entrada:	
*		- a fila e o elemento a ser adicionado
*
*	Assertivas De Saída:	
*		- A fila com mais m nó no fim
*
***********************************************************************/
void Enqueue(No * no);

/***********************************************************************
*   Função: RoundRobin
*
*	Descrição: Executa o escalonamento de processos como ROundRObin com a fatia d etempo determinada em QUANTUM
*
*	Assertivas De Entrada:	
*		- a fila 
*
*
***********************************************************************/
void RoundRobin(void);