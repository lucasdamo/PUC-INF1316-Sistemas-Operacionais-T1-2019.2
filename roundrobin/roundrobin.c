/***************************************************************************
*  Módulo de implementação: RoundRobin
*
*  Projeto: PUC Rio INF1316 Sistemas Operacionais T1 2019.2
*  Gestor:  LES/DI/PUC-Rio
*  Autores: Luiza Del Negro
*			Lucas Rebello Damo
*
*  Histórico de evolução:
*     Versão	Data		Observações
*     1			20/10/2019	Versão inicial utilizando os moldes de funções de fila de prioridades feitas por Lucas Damo
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
#include "fila.h"
#define DEBUG 1
#define NUM_PALAVRAS_ESPERADO 2
int PROCESSOS_FINALIZADOS = 0;


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
	palavraprd = strtok_r(argumentos[2], "PR=", &saveptr2);
	strcpy(cmm->com, argumentos[1]);
	sscanf(palavraprd, "%d", &cmm->prioridade);
	write(p[1], cmm, sizeof(Comando));
	#ifdef DEBUG
	printf("Escrito comando %s prioridade %d\n", cmm->com, cmm->prioridade);
	#endif

}


pid_t iniciaNovoProcesso(char * cmd){
	int fFork, msgid;
	key_t key;
	key = ftok("novoproc", 65); //cria uma key IPC
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


void signusr1_handler(int sig){
	printf("Signal received %d\n", sig);
	kill(getpid(), SIGCONT);
	executando = 0;
}


void resumeProcesso(pid_t pid){
	kill(pid, SIGCONT);
}

void pausaProcesso(pid_t pid){
	kill(pid, SIGSTOP);
}
/*Ira ler a lista de processos pela primeira vez de acordo com o enunciado
os processos serao iniciados e atuarao por um segundo(QUANTUM)
se nao acabarem nesse quantum, voltam para a fila para serem continuados depois pelo parenthandler
filho vai interpretar o comando de cada linha*/
void childHandler(void){
	int a;
	
	Comando * buff;
	int nBytes, i, flagFila;
	pid_t pidBuffer;
	char cmdBuffer[255];
	buff = (Comando *)malloc(sizeof(Comando));
	executando = 0;
	signal(SIGUSR1, signusr1_handler);
	printf("Escalonador pid = %d\n", getpid());
	while( ( nBytes = read(p[0], buff, sizeof(Comando)) ) != 0 ){// enquanto tiverem elementos na lista, vao sendo inseridos
		printf("Comando %s recebido \n", buff->com);
		insereFilaCmd(buff->com);	// Insere na fila
	}
		/*Faz um fork para cada elemento da lista, assim crianod um pai e um filho. Cada pai controla a execucao do seu filho.
		Executa todo elemento da fila pelo menos uma vez*/
		/*pega o primeiro elemento da fila sempre*/
		for(i=0;i<pFila->contador;i++){/*Agora tem a qtd total de elementos na fila*/
			flagFila = retiraPrimeiro(cmdBuffer, &pidBuffer,);
			if(flagFila!=-1){
				pidBuffer=fork();/*cria um processo pai e filho para o pai controlar o filho*/
				if(pidBuffer==0){/*eh filho, inicia o pocesso*/
					execv(cmdBuffer, NULL);
					//a = execl(cmd, cmd, "", "", NULL);	
					exit(1);
				}
				if(pidBuffer>0){/*eh pai, para o processo dps de 1 seg*/
					sleep(QUANTUM);/*Pai tem que esperar o filho*/
					/* antes de parar tem que ver se acabou*/
					int status;
					int wpid = waitpid(pidBuffer, &status, WNOHANG);/*analisa o status do filho*/
					if(wpid && WIFEXITED(status) && (WEXITSTATUS(status) == 0)){
						printf("FIM-Processo %s finalizado \n", cmdBuffer);
						kill(pidBuffer, SIGKILL);// termina o processo
						printf("FIM-Dentro dos finalizados, passa para o proximo = %s\n", cmdBuffer);
						PROCESSOS_FINALIZADOS=PROCESSOS_FINALIZADOS+1;
					}
					else{
						kill(aux->pid, SIGSTOP);// para o processo
				/*se nao acabou,coloca no final*/
						insereFilaPid(pidBuffer)
					}// fim else
				}//fim pai
				else{
					printf("erro ao executar filho");
					exit(1);
				}// fim se erro filho
		}/*fim if flag!=-1*/
	}/*fim for*/
	
	/*agora, para os processos que nao terminaram, temos que fazer terminar*/
	
	while(PROCESSOS_FINALIZADOS<pFila->contador){
		flagFila = retiraPrimeiro(cmdBuffer, &pidBuffer);// tira primeiro elemento da fila
		kill(aux->pid, SIGCONT);
		sleep(QUANTUM);
		int status;
		int wpid = waitpid(pidBuffer, &status, WNOHANG);/*analisa o status do filho*/
		if(wpid && WIFEXITED(status) && (WEXITSTATUS(status) == 0)){/*SE O FILHO ACABOU, ACABOU TBM*/
			printf("FIM-Processo %s finalizado \n", aux->comandodoprograma);
			kill(aux->pid, SIGKILL);// termina o processo
			printf("FIM-Dentro dos finalizados, passa para o proximo = %s\n", aux->comandodoprograma);
			PROCESSOS_FINALIZADOS=PROCESSOS_FINALIZADOS+1;
		}
		else{
			printf("interrompendo o processo %s \n", aux->comandodoprograma);	
			kill(pidBuffer, SIGSTOP);// para o processo
			/*se nao acabou, tira e passa pro final*/
			insereFilaPid(pidBuffer);
		}
	}//fim while
	
}/*fim funcao*/


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
		sleep(1); /* Enunciado: O interpretador irá ler de exec.txt quais são os programas a serem executados, e deverá iniciá-los exatamente na ordem em que aparecem nesse arquivo, com um intervalo de 1 segundo entre cada um deles */
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
	inicializaFila();
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



