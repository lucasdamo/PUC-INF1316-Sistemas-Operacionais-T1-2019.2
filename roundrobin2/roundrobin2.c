/***************************************************************************
*  Módulo de implementação: Interprador para o escalonador de tipo PRIORIDADE
*
*  Projeto: PUC Rio INF1316 Sistemas Operacionais T1 2019.2
*  Gestor:  LES/DI/PUC-Rio
*  Autores: Lucas Rebello Damo
*			Luiza Del Negro
*
*  Histórico de evolução:
*     Versão	Data		Observações
*	  2			19/10/2019	Escalonador permite preempção	
*     1			05/10/2019	Versão inicial
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
#include "../fila/fila.h"
#define DEBUG 1
#define NUM_PALAVRAS_ESPERADO 3 // Numero de palavras esperado em cada linha do arquivo txt inputado
#define TRUE 1
#define FATIA_TEMPO 1  // Fatia de tempo em segundos
 
typedef struct comando {
	char com[255];
	int prioridade;
} Comando;

int p[2];
pid_t executando;


void interpretaComandos(char * linha){
	char * palavra, * palavraprd;
	char * saveptr, * saveptr2; // Variavel para uso interno do strtok
	char * argumentos[NUM_PALAVRAS_ESPERADO];
	Comando * cmm;
	int contaPalavra = 0;

	cmm = (Comando*)malloc(sizeof(Comando));
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
	palavraprd = strtok_r(argumentos[2], "PR=", &saveptr2);
	strcpy(cmm->com, argumentos[1]);
	sscanf(palavraprd, "%d", &cmm->prioridade);
	write(p[1], cmm, sizeof(Comando));
	#ifdef DEBUG
	printf("Escrito comando %s prioridade %d\n", cmm->com, cmm->prioridade);
	#endif

}

void signusr1_handler(int sig){
	printf("Signal received %d\n", sig);
	kill(getpid(), SIGCONT);
	executando = 0;
}

pid_t iniciaNovoProcesso(char * cmd){
	int fFork, msgid;
	key_t key;
	key = ftok("novoproc", 65); 
	msgid = msgget(key, 0666 | IPC_CREAT); 
	printf("getpid = %d\n", getpid());
	fFork = fork();
	struct msg_buffer{
		long mtype;
		pid_t msg;
	} message;
	if(fFork == 0){
		int fFork2;
		pid_t paiPid = getppid();
		fFork2 = fork();
		if(fFork2 == 0){
			int a;
			printf("Execl pid %d\n", getpid());
			a = execl(cmd, cmd, "", "", NULL);
			fprintf(stderr, "Não foi possivel executar o programa %s\n", cmd);
			exit(EXIT_FAILURE);
		}
		else{
			int status;
			message.msg = fFork2;
			message.mtype = 1;
			printf("Waiter pid = %d\n", getpid());
			printf("Get ppid = %d\n", getppid());
			printf("Waiting for %d\n", fFork);
			printf("ESCREVENDO FFORK2 %d!\n", fFork2);
			msgsnd(msgid, &message, sizeof(message),0);
			//kill(getppid(), SIGCONT);
			waitpid(fFork, &status, 0);
			if(WIFEXITED(status)){
				printf("EXITED!! %d paipid = %d\n", WEXITSTATUS(status), paiPid);
				kill(paiPid, SIGCONT);
				kill(paiPid, SIGUSR1);
				printf("SIGCONT + SIGUSR ENVIADOS PARA %d\n", paiPid);
			}
			printf("MORRE\n");
			kill(getpid(), SIGTERM);
		}
	}
	else {
		pid_t forkBuff;
		msgrcv(msgid, &message, sizeof(message), 1, 0);
		printf("fFork = %d\n", message.msg);
		return message.msg;
	}
}

void resumeProcesso(pid_t pid){
	kill(pid, SIGCONT);
}

void pausaProcesso(pid_t pid){
	kill(pid, SIGSTOP);
}

void childHandler(){
	Comando * buff;
	int nBytes, i, flagFila, flagFork, status, prioridadeRodando;
	int prioridadeBuffer;
	pid_t pidBuffer;
	char cmdBuffer[255];
	inicializaFila();
	buff = (Comando *)malloc(sizeof(Comando));
	prioridadeRodando = 100; // Para ser menos importante do que qualquer processo
	executando = 0;
	signal(SIGUSR1, signusr1_handler);
	printf("Escalonador pid = %d\n", getpid());
	while(TRUE){
		while( ( nBytes = read(p[0], buff, sizeof(Comando)) ) != 0 ){
			printf("Comando %s recebido com prioridade %d\n", buff->com, buff->prioridade);
			// Insere na fila
			insereFilaCmd(buff->prioridade, buff->com);
			if(buff->prioridade < prioridadeRodando || executando == 0) break; // Para de receber comandos para alterar execução
		}
		printf("Executando %d prioridadeRodando %d\n", executando, prioridadeRodando);
		// Retira primeiro da fila e espera
		flagFila = retiraPrimeiro(cmdBuffer, &pidBuffer, &prioridadeBuffer);
		if(flagFila != -1){
			// Foi retirado um nó da fila
			if(prioridadeBuffer < prioridadeRodando || executando == 0){
				// Necessario mudar o processo sendo executado
				if (executando != 0){
					printf("Parando processo %d com prioridade %d\n", executando, prioridadeRodando);
					pausaProcesso(executando);
					insereFilaPid(prioridadeRodando, executando);
				}
				prioridadeRodando = prioridadeBuffer;
				if(flagFila == 0){
					// Comando recebido, iniciar novo processo
					printf("Iniciando processo %s\n", cmdBuffer);
					executando = iniciaNovoProcesso(cmdBuffer);
				}
				else if(flagFila == 1){
					// PID recebido, resumir processo parado
					printf("Resumindo processo %d\n", pidBuffer);
					resumeProcesso(pidBuffer);
					executando = pidBuffer;
				}
			}
			else {
				printf("Processo nao vai ser executado, voltando para a lista\n");
				if(flagFila == 0) insereFilaCmd(prioridadeBuffer, cmdBuffer);
				else if (flagFila == 1) insereFilaPid(prioridadeBuffer, pidBuffer);
			}
		}
		else{
			printf("Fim da fila e dos comandos... Saindo do programa\n");
			return;
		}
		printf("Parando escalonador %d\n", getpid());
		kill(getpid(), SIGSTOP);
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
		printf("Resumindo escalonador %d\n", escl);
		kill(escl, SIGCONT);
		sleep(1); /* Enunciado: O interpretador irá ler de exec.txt quais são os programas a
					serem executados, e deverá iniciá-los exatamente na ordem em que aparecem nesse arquivo,
					com um intervalo de 1 segundo entre cada um deles */
	}
	free(linha); // getLine aloca memoria automaticamente

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
	int status;

	if(pipe(p) < 0){
		fprintf(stderr, "Erro ao realizar o pipe para a comunicacao\n");
		exit(EXIT_FAILURE);	
	}

	fp = fopen("exec.txt", "r");
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
		close(p[1]);
		childHandler();
	}
	else {
		/* Processo pai */
		close(p[0]);
		kill(SIGSTOP, flagFork);
		parentHandler(fp, flagFork);
		close(p[1]);
		waitpid(flagFork, &status, 0);
	}

	return 0;
}



