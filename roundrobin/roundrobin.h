

void VetorDeProcessos( char* file);

void GerenciaOutput(void);

/*funcoes para ver os processos que estao iniciados/finalizdos/em espera*/
void VerProcessosFinalizados(void);

void VerProcessosProntos(void);

void VerProcessosEmEspera(void);


/*O escalonador */

void RoundRobin( void);


/*funcoes auxiliares */

void InitWaitInMemory(void);

void AnalisaProcessosEmEspera(void);

void poeProcessoEmEspera(int pid);

void remProcessoEmEspera(int pid);


void AnalisaVetorDeProcessos(void);

