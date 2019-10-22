/***************************************************************************
*  Módulo de implementação: Interprador para o escalonador de tipo REAL-TIME
*
*  Projeto: PUC Rio INF1316 Sistemas Operacionais T1 2019.2
*  Gestor:  LES/DI/PUC-Rio
*  Autores: Lucas Rebello Damo
*
*  Histórico de evolução:
*     Versão	Data		Observações
*     1         21/10/2019  Versão inicial
*
***************************************************************************/
#include <stdio.h>	
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <ctype.h>
#include <sys/sem.h>
#include <fcntl.h> 
#include <sys/shm.h>
#include <sys/stat.h>

#define DEBUG 1
#define TRUE 1
#define TAM_VETOR 60
#define NUM_PALAVRAS_ESPERADO 4 // Numero de palavras esperado em cada linha do arquivo txt inputado

// inicializa o valor do semáforo
int setSemValue(int semId);
// remove o semáforo
void delSemValue(int semId);
// operação P
int semaforoP(int semId);
//operação V
int semaforoV(int semId);

union semun
{	
	int val;
	struct semid_ds *buf;
	ushort *array;
};

typedef struct programa {
	char com[255];	  // Comando para inicio do programa
	pid_t pid;	  // Pid para o processo, caso o programa já tenha iniciado
	int status;	  // 1 = Concluido 0 = Caso contrário
	int inicio;   // Parametro I do exec.txt MOMENTO INICIO
	int duracao;  // Parametro D do exec.txt TEMPO DURAÇÃO
} Programa;

int segmento;
Programa * p;
int semId;

void interpretaComandos(char * linha){
	char * palavra, * palavraprd;
	char * saveptr, * saveptr2; // Variavel para uso interno do strtok
	char * argumentos[NUM_PALAVRAS_ESPERADO];
	Programa * prog;
	int contaPalavra = 0, i;

	prog = (Programa*)malloc(sizeof(Programa));
	palavra = strtok_r(linha, " \n", &saveptr);
	while(palavra != NULL){
		argumentos[contaPalavra] = palavra;
		contaPalavra++;
		if(contaPalavra > NUM_PALAVRAS_ESPERADO){
			fprintf(stderr, "Numero de parâmetros inesperado!\n");
			exit(EXIT_FAILURE);	
		}
		palavra = strtok_r(NULL, " \n", &saveptr);
	}
	if(strcmp(argumentos[0], "Run") != 0){
		fprintf(stderr, "Comando %s não começa com Run, e sim com %s\n", linha, argumentos[0]);
		exit(EXIT_FAILURE);	
	}
	strcpy(prog->com, argumentos[1]);
	palavraprd = strtok_r(argumentos[2], "I=", &saveptr2);
	sscanf(palavraprd, "%d", &prog->inicio);
	palavraprd = strtok_r(argumentos[3], "D=", &saveptr2);
	sscanf(palavraprd, "%d", &prog->duracao);
	prog->status = 0;
	prog->pid = 0;
	for(i = prog->inicio; i < prog->duracao; i++){
		semaforoP(semId);
		p[i] = *prog;
		semaforoV(semId);
		
	}

}


void parentHandler(FILE *fp, pid_t escl){
	char * linha = NULL; // Ponteiro para a linha a ser lida
	size_t tam = 0;
	ssize_t charLidos; // Quantidade de caracteres lidos
	#ifdef DEBUG
		printf("Processo pai iniciado com pid %d\n", getpid());
	#endif

	while ((charLidos = getline(&linha, &tam, fp)) != -1){	
		interpretaComandos(linha);
		sleep(1); /* Enunciado: O interpretador irá ler de exec.txt quais são os programas a
					serem executados, e deverá iniciá-los exatamente na ordem em que aparecem nesse arquivo,
					com um intervalo de 1 segundo entre cada um deles */
		semaforoP(semId);
		printf("Testando mem comp; %s d %d i %d\n", p[1].com, p[1].duracao, p[1].inicio);
		semaforoV(semId);
	}
	free(linha); // getLine aloca memoria automaticamente

}

void childHandler(){
	Programa * buff;
	int time[60] = {0};
	int nBytes;

	buff = (Programa *)malloc(sizeof(Programa));
	printf("Filho iniciado\n");
	while(TRUE){

	}

}

/***********************************************************************
*
*	Descrição: Ponto de entrada do programa
*
*	Assertivas De Entrada:	
*		- Existe um arquivo chamado exec.txt na mesma pasta do programa, contendo os comandos
*
***********************************************************************/
int main(int argc, char *argv[]){
	FILE *fp; // Ponteiro para o arquivo a ser aberto
	pid_t flagFork;
	int status, segmento;

	segmento = shmget(IPC_PRIVATE, TAM_VETOR * sizeof(Programa), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	p = (Programa *)shmat(segmento, 0, 0);
	fp = fopen("exec.txt", "r");
	semId = semget (8752, 1, 0666 | IPC_CREAT);

	setSemValue(semId);
	if (fp == NULL){
		fprintf(stderr, "Erro ao abrir o arquivo exec.txt\n");
		exit(EXIT_FAILURE);	
	}

	flagFork = fork();
	if (flagFork < 0){
		fprintf(stderr, "Erro ao fazer o fork\n");
		exit(EXIT_FAILURE);	
	}
	else if (flagFork == 0){
		/* Processo filho */
		childHandler();
	}
	else {
		/* Processo pai */
		kill(SIGSTOP, flagFork);
		parentHandler(fp, flagFork);
		waitpid(flagFork, &status, 0);
	}
	shmdt(p);
	shmctl(segmento, IPC_RMID, 0);
	delSemValue(semId);
	return 0;
}

int setSemValue(int semId)
{
	union semun semUnion;
	semUnion.val = 1;
	return semctl(semId, 0, SETVAL, semUnion);
}
void delSemValue(int semId)
{
	union semun semUnion;
	semctl(semId, 0, IPC_RMID, semUnion);
}
int semaforoP(int semId)
{
	struct sembuf semB;
	semB.sem_num = 0;
	semB.sem_op = -1;
	semB.sem_flg = SEM_UNDO;
	semop(semId, &semB, 1);
	return 0;
}
int semaforoV(int semId)
{
	struct sembuf semB;
	semB.sem_num = 0;
	semB.sem_op = 1;
	semB.sem_flg = SEM_UNDO;
	semop(semId, &semB, 1);
	return 0;
}