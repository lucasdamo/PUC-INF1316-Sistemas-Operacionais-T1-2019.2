/***************************************************************************
*  Módulo de interface: RoundRobin
*
*  Projeto: PUC Rio INF1316 Sistemas Operacionais T1 2019.2
*  Gestor:  LES/DI/PUC-Rio
*  Autores: Luiza Del Negro
*           Lucas Rebello Damo
*  Histórico de evolução:
*     Versão	Data		Observações
*     1			27/10/2019	Versão inicial
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

typedef union cmdpid Cmdpid;
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
*   Função: insereFilaCmd
*
*	Descrição: Insere ordenadamente na fila, uma nova struct contendo
*	o comando.
*
*	Parâmetros: 
*		- Comando: String de até 255 caracteres
*
*	Assertivas De Entrada:	
*		- A fila já foi inicializada
*
*	Assertivas De Saída:	
*		- A fila possui mais um nó recém inserido
*
***********************************************************************/
void insereFilaCmd(char * comando);

/***********************************************************************
*   Função: insereFilaPid
*
*	Descrição: Insere ordenadamente na fila, uma nova struct contendo
*	tanto o numero do processo.
*
*	Parâmetros: 
*		- Pid: Pid do processo
*
*	Assertivas De Entrada:	
*		- A fila já foi inicializada
*
*	Assertivas De Saída:	
*		- A fila possui mais um nó recém inserido
*
***********************************************************************/
void insereFilaPid(pid_t pid);

/***********************************************************************
*   Função: retiraPrimeiro
*
*	Descrição: Pega o processo de maior prioridade na fila
*
*	Parâmetros: 
*		- Comando: Ponteiro para char, onde será armazenado a string
*		caso o nó contenha um comando
*		- Pid: Ponteiro para uma região do tipo pid_t onde será arma-
*		zenado caso o nó contenha um número de processo
*
*	Retorno: 
*		- 0, caso o nó contenha comando
*		- 1, caso o nó contenha número de processo
*		- Ou então, -1 caso a fila esteja vazia
*
*	Assertivas De Entrada:	
*		- A fila já foi inicializada
*
*	Assertivas De Saída:	
*		- A fila possui um nó a menos
*
***********************************************************************/
int retiraPrimeiro(char * comando, pid_t * pid);

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

