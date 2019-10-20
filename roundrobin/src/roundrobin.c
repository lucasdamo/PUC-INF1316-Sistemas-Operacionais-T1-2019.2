#include "roundrobin.h"

struct processos{
	int pid;
	char * nome;
	int status;/* 0= pronto 1=esperando 2=terminou*/
};

Processos ** processos;
int num_processos = 0;
int processos_finalizados = 0;
int * waitingmemory;

int main(int argc, char* argv[]) {

	int i,j=0;
	int indice;
	FILE* entrada;
	FILE* saida;

	printf("Executando o escalonador round-robin: \n");
	// entrada = fopen("/Users/luiza.fernandes/Luiza/Projects/PUC-INF1316-Sistemas-Operacionais-T1-2019.2/roundrobin/entradaroundrobin.txt", "r");
	// saida = fopen("/Users/luiza.fernandes/Luiza/Projects/PUC-INF1316-Sistemas-Operacionais-T1-2019.2/roundrobin/saidaroundrobin.txt", "w");

/*Verifica quantos programas são, coloca eles em ordem*/
	printf("Criando vetor de processos: \n");
	VetorDeProcessos(argv[2]);
	GerenciaOutput();
	RoundRobin();

	// fclose(entrada);
	// fclose(saida);

	return 0;

}


void VetorDeProcessos( char* file){
	int i = 0;
	char execCommand[4];
	char program[50];

	FILE * arq = fopen(file,"r");

	if(arq==NULL) {
		printf("ERRO AO ABRIR O ARQUIVO!");
		exit(1);
	}
	processos = (Processos**)malloc(sizeof(Processos*));
	
	while(fscanf(arq,"%s %s",execCommand,program)==2){
		Processos * atual=(Processos*)malloc(sizeof(Processos));
		atual->pid = 0;
		atual->nome = (char*)malloc(sizeof(char)*strlen(program));
		atual->status = 0;/* 0= pronto 1=esperando 2=terminou*/
		processos[i] = atual;
		i++;
	}
	fclose(arq);
	num_processos = i;

	
}

void GerenciaOutput(void){
	int output;
	char nome[50];

	strcpy(nome,"saidaroundrobin.txt");
}

void RoundRobin(){
	int i,j=0;
	Processos * p;
	while(processos_finalizados<num_processos){
		VerProcessosEmEspera();/*Enquanto nao termina, vemos os processos que ainda nao terminaram*/
		i = j%num_processos;/*usamos o i como indice para assumir uma incrementaçao que tende ao infinito */
		p = processos[i];
		if(p->pid==0){/*vemos se ja esta pronto*/
			p->pid=fork();
			if(p->pid!=0){/*pai*/
				sleep(fatia);//espera 
				printf("Interrompendo: %s\n",p->nome);
				kill(p->pid,SIGSTOP);
			}
			else{
				printf("Executando: %s \n",p->nome);
				execve(p->nome,NULL,NULL);
			}
		}
		else{
			if(p->status==2){
				int status;
				int wpid=waitpid(p->pid,&status,WNOHANG);//wnohang nao deixa que checar o filho suspenda quem o chamou 
				printf("Executando: %s \n",p->nome);
				kill(p->pid,SIGCONT);
				sleep(fatia);
				printf("Interrompendo: %s\n",p->nome);
				kill(p->pid,SIGSTOP);
				if(wpid&&(p->status!=2)&&WIFEXITED(status)&&(WEXITSTATUS(status)==0)){
					printf("Processo %s Finalizado! :D \n ",p->nome);
					p->status=2;
					kill(p->pid,SIGKILL);
					processos_finalizados=processos_finalizados+1;
				}

			}
		}
	j++;
	VerProcessosFinalizados();
	VerProcessosProntos();
	VerProcessosEmEspera();
	}
}


void InitWaitInMemory(void){
	int segmento;
	int i;
	int id = 3333;
	segmento = shmget(id, sizeof(int)*10, IPC_CREAT | 0666);
	waitingmemory = (int*)shmat(segmento,NULL,0);
	for(i = 0;i<10;i++){
		waitingmemory[i]=0;
	}
}

void AnalisaProcessosEmEspera(void){
	int i;
	for(i = 0;i<10;i++){
		if(waitingmemory[i]>0)
			poeProcessoEmEspera(waitingmemory[i]);
			waitingmemory[i]=0;
		if(waitingmemory[i]<0)
			remProcessoEmEspera(waitingmemory[i]);
			waitingmemory[i] = 0;
	}

}

void poeProcessoEmEspera(int pid){
	int i;
	for(i = 0;i<num_processos;i++){
		if(processos[i]->pid==pid){
			processos[i]->status=1;
		}
	}
}

void remProcessoEmEspera(int pid){
	int i;
	int proc = pid*-1;
	for(i = 0; i< num_processos;i++){
		if(processos[i]->pid==proc)
			processos[i]->status = 0;
	}
}

void AnalisaVetorDeProcessos(void){
	int i;
	printf("Processos: :\n");
	for(i = 0;i<num_processos;i++){
		printf("\n Processo %d: pid=%d , nome:%s  \n", i+1, processos[i]->pid, processos[i]->nome);
	}
}

void VerProcessosFinalizados(void){
	int i;
	if(processos_finalizados!=0){
		printf("Processos Finalizados: \n");
		if(processos[i]->status==2)
			printf("nome: %s , pid: %d", processos[i]->nome, processos[i]->pid);
	}
	else
		printf("Nenhum processo terminou ainda \n");
}

void VerProcessosProntos(void){
	int i;
	if(processos_finalizados!=num_processos){
		printf("Processos Prontos\n");
		for(i = 0;i<num_processos;i++){
			if(processos[i]->status==0)
				printf("nome: %s , pid: %d", processos[i]->nome, processos[i]->pid);
		}
	}
}
void VerProcessosEmEspera(void){
	int i;
	int espera = 0;
	if(processos_finalizados!=num_processos){
		printf("Processos Em Espera\n");
		for(i = 0;i<num_processos;i++){
			if(processos[i]->status==1)
				espera = espera+1;
				printf("nome: %s , pid: %d", processos[i]->nome, processos[i]->pid);
		}
	}
	if(espera==0)
		printf("Nao tem processos em espera");
 }

