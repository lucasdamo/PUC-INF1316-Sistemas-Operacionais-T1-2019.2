/***************************************************************************
*  Módulo de implementação: Interprador para o escalonador de tipo ROUNDROBIN
*
*  Projeto: PUC Rio INF1316 Sistemas Operacionais T1 2019.2
*  Gestor:  LES/DI/PUC-Rio
*  Autores: Lucas Rebello Damo
*			Luiza Del Negro
*
*  Histórico de evolução:
*     Versão	Data		Observações
*     1			27/10/2019	Versão inicial
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
#include "./fila/fila.h"
#define DEBUG 1
#define NUM_PALAVRAS_ESPERADO 2 // Numero de palavras esperado em cada linha do arquivo txt inputado
#define TRUE 1
#define FATIA_TEMPO 1  // Fatia de tempo em segundos
 
typedef struct comando {
	char com[255];
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
	strcpy(cmm->com, argumentos[1]);
	write(p[1], cmm, sizeof(Comando));
	#ifdef DEBUG
	printf("Escrito comando %s\n", cmm->com);
	#endif

}

void signusr1_handler(int sig){
	Comando * buff;
	int nBytes;
	buff = (Comando *)malloc(sizeof(Comando));
	printf("Signal received %d\n", sig);
	nBytes = read(p[0], buff, sizeof(Comando));
	printf("Comando %s recebido\n", buff->com);
	insereFilaCmd(buff->com);

}

pid_t iniciaNovoProcesso(char * cmd){
	int fFork;
	fFork = fork();
	if(fFork < 0){
		fprintf(stderr, "Não foi possivel fazer o fork ao tentar iniciar o prograam %s\n", cmd);
		exit(EXIT_FAILURE);
	}
	if(fFork == 0){
		int a;
		#ifdef DEBUG
		printf("Executando processo... %d\n", getpid());
		#endif
		a = execl(cmd, cmd, "", "", NULL);
		fprintf(stderr, "Não foi possivel executar o programa %s\n", cmd);
		exit(EXIT_FAILURE);
	}
	else{
		return fFork;
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
	int nBytes, i, flagFila, flagFork, status;
	pid_t pidBuffer;
	char cmdBuffer[255];
	buff = (Comando *)malloc(sizeof(Comando));
	executando = 0;
	printf("Escalonador pid = %d\n", getpid());
	while(TRUE){
		printf("Executando %d\n", executando);
		// Retira primeiro da fila e espera
		flagFila = retiraPrimeiro(cmdBuffer, &pidBuffer);
		if(flagFila != -1){
			// Foi retirado um nó da fila
			// Necessario mudar o processo sendo executado
			if (executando != 0){
				printf("Parando processo %d\n", executando);
				pausaProcesso(executando);
				insereFilaPid(executando);
			}
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
		// else{
		// 	printf("Fim da fila e dos comandos... Saindo do programa\n");
		// 	return;
		// }
		sleep(1);
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
		kill(escl, SIGUSR1);
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
	signal(SIGUSR1, signusr1_handler);

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
		inicializaFila();
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



