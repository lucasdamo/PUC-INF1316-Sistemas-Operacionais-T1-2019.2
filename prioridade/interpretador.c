/***************************************************************************
*  Módulo de implementação: Interprador para o escalonador de tipo PRIORIDADE
*
*  Projeto: PUC Rio INF1316 Sistemas Operacionais T1 2019.2
*  Gestor:  LES/DI/PUC-Rio
*  Autores: Lucas Rebello Damo
*           Luiza Del Negro
*
*  Histórico de evolução:
*     Versão	Data		Observações
*     1			05/10/2019	Versão inicial
*
***************************************************************************/
#include <stdio.h>	
#include <stdlib.h>
#include <string.h>
#define NUM_PALAVRAS_ESPERADO 3 // Numero de palavras esperado em cada linha do arquivo txt inputado
 

void interpretaComandos(char * linha){
	char * palavra;
	char * saveptr; // Variavel para uso interno do strtok
	char * argumentos[NUM_PALAVRAS_ESPERADO];
	int contaPalavra = 0, i;
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
}

/***********************************************************************
*
*	Descrição: Ponto de entrada do programa
*
*	Assertivas De Entrada:
*		-Chamada do programa contendo um parâmetros:
*			1º: Path para o arquivo contendo os comandos
*
***********************************************************************/
int main(int argc, char *argv[]){
	FILE *fp; // Ponteiro para o arquivo a ser aberto
	char * linha = NULL; // Ponteiro para a linha a ser lida
	size_t tam = 0;
	ssize_t charLidos; // Quantidade de caracteres lidos
	if(argc != 2){
		fprintf(stderr, "Chamada do programa deve conter apenas o path para o arquivo contendo os comandos\n");
		exit(EXIT_FAILURE);
	}
	fp = fopen(argv[1], "r");
	if (fp == NULL)
		return(1); // Erro ao abrir o arquivo
	while ((charLidos = getline(&linha, &tam, fp)) != -1){
		interpretaComandos(linha);
	}
	free(linha); // getLine aloca memoria automaticamente

	return 0;
}



