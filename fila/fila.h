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

/***********************************************************************
*
*	Descrição: Insere ordenadamente na fila, uma nova struct contendo
*	tanto o numero do processo quanto sua prioridade.
*
*	Parâmetros: 
*		- Prioridade: Prioridade associada ao processo
*		- Pid: Numero que identifica o processo
*
*	Assertivas De Entrada:	
*		- A fila já foi inicializada
*
*	Assertivas De Saída:	
*		- A fila possui mais um nó recém inserido
*
***********************************************************************/
void insereFila(int prioridade, pid_t pid);

/***********************************************************************
*
*	Descrição: Pega o processo de maior prioridade na fila
*
*	Retorno: 
*		- 0, caso a fila esteja vazia
*		- Ou então, retorna o numero que identifica o processo de maior
*		prioridade na fila
*
*	Assertivas De Entrada:	
*		- A fila já foi inicializada
*
*	Assertivas De Saída:	
*		- A fila possui um nó a menos
*
***********************************************************************/
pid_t retiraPrimeiro(void);

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
