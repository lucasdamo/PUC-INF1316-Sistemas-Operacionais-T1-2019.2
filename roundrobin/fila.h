/***************************************************************************
*  Módulo de interface: Fila
*
*  Projeto: PUC Rio INF1316 Sistemas Operacionais T1 2019.2
*  Gestor:  LES/DI/PUC-Rio
*  Autores: Lucas Rebello Damo
*
*  Histórico de evolução:
*     Versão	Data		Observações
*     1			10/10/2019	Versão inicial
*
***************************************************************************/

typedef union cmdpid Cmdpid;
typedef struct no No;
typedef struct fila Fila;

/***********************************************************************
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


static void insereFila( char * comando_programa,char * nome_programa);



/***********************************************************************
*
*	Descrição: Pega o processo de maior prioridade na fila
*
*	Parâmetros: 
*		- Comando: Ponteiro para char, onde será armazenado a string
*		caso o nó contenha um comando
*		- Pid: Ponteiro para uma região do tipo pid_t onde será arma-
*		zenado caso o nó contenha um número de processo
*		- Prioridade: Ponteiro para uma região do tipo int onde será
*		armazenado a prioridade do nó retirado
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
// int retiraPrimeiro(char * comando,, int * prioridade);

/***********************************************************************
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
