/***************************************************************************
*  Módulo de implementação: Interprador para o escalonador de tipo PRIORIDADE
*
*  Projeto: PUC Rio INF1316 Sistemas Operacionais T1 2019.2
*  Gestor:  LES/DI/PUC-Rio
*  Autores: Lucas Rebello Damo
*
*  Histórico de evolução:
*     Versão	Data		Observações
*     1			05/10/2019	Versão inicial
*
***************************************************************************/
#include <stdio.h>	
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include "../fila/fila.h"
#define DEBUG 1
#define NUM_PALAVRAS_ESPERADO 3 // Numero de palavras esperado em cada linha do arquivo txt inputado
#define TRUE 1
 
typedef struct comando {
	char com[255];
	int prioridade;
} Comando;

int p[2];


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




void childHandler(){
	#ifdef DEBUG
		printf("Processo filho iniciado com pid %d\n", getpid());
	#endif
	Comando * buff;
	int nBytes, i;

	buff = (Comando *)malloc(sizeof(Comando));

	while( ( nBytes = read(p[0], buff, sizeof(Comando)) ) != 0 ){
		#ifdef DEBUG
			printf("Comando %s recebido com prioridade %d\n", buff->com, buff->prioridade);
		#endif
	}
	sleep(5);
	for(i=0; i<1000; i++){}
	
	

}

void parentHandler(FILE *fp){
	char * linha = NULL; // Ponteiro para a linha a ser lida
	size_t tam = 0;
	ssize_t charLidos; // Quantidade de caracteres lidos
	#ifdef DEBUG
		printf("Processo pai iniciado com pid %d\n", getpid());
	#endif

	while ((charLidos = getline(&linha, &tam, fp)) != -1){
		interpretaComandos(linha);

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
		parentHandler(fp);
	}

	return 0;
}



