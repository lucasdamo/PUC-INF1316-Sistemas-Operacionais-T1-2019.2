#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef struct processos Processos;

struct processos{
	int pid;
	char * nome;
	int status;/*bool*/
};

Processos ** processos;
int num_processos = 0;
int processos_finalizados = 0;
int * waitingmemory;

void InitWaitInMemory(){
	int segmento;
	int i;
	int id=3333;
	segmento = shmget(id, sizeof(int)*10, IPC_CREAT | 0666);
	waitingmemory=(int*)shmat(segmento,NULL,0);
	for(i=0;i<10;i++){
		waitingmemory[i]=0;
	}
}
void remProcessoEmEspera(){

}

void VerProcessosEmEspera(){
	int i;
	for(i=0;i<10;i++){
		if(waitingmemory[i]>0)
			setProcessoEmEspera(waitingmemory[i]);
			waitingmemory[i]=0;
		if(waitingmemory[i]<0)
			remProcessoEmEspera(waitingmemory[i]);
			waitingmemory[i]=0;
	}

}


void VetorDeProcessos( char* file){
	int i = 0;
	char execCommand[4];
	char program[50];

	FILE * input = fopen(file,"r");

	if(input==NULL) {
		printf("ERRO AO ABRIR O ARQUIVO!");
		exit(1);
	}
	processos=(Processos**)malloc(sizeof(Processos*));
	
	while(fscanf(input,"%s %s",execCommand,program)==2){
		Processos * atual=(Processos*)malloc(sizeof(Processos));
		atual->pid= 0;
		atual->nome=(char*)malloc(sizeof(char)*strlen(program));
		atual->status= 0;/* 0= pronto 1=esperando 2=terminou*/
		processos[i]=atual;
		i++;
	}
	fclose(input);
	num_processos=i;

}


int main(int argc, char* argv[]) {

	int i,j=0;
	int indice;
	int fatia_de_tempo = 1;
	FILE* entrada;
	FILE* saida;

	printf("Executando o escalonador round-robin...\n");
	entrada = fopen("/Users/Rito/Downloads/SC/entradaroundrobin.txt", "r");
	saida = fopen("/Users/Rito/Downloads/SC/saidaroundrobin.txt", "w");
	

/*Verifica quantos programas são, coloca eles em ordem*/
	printf("Criando vetor de processos: \n");
	VetorDeProcessos(entrada);
	printf("Processos encontrados:\n");
	for(i=0;i<num_processos;i++){
		printf("Processo %d -  nome: %s - PID: %d \n",i,processos[i]->nome,processos[i]->pid);
	}

/*Inicia a criação de processos*/
/*Verifica se tem processos para rodar*/
	while(processos_finalizados<num_processos){
		VerProcessosEmEspera();
		indice=j%num_processos;/*o indice pode ser dividido  em um num quase infinito */
		


	}



	fclose(entrada);
	fclose(saida);

	return 0;

}
