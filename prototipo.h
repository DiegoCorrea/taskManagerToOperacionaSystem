# ifndef _PROTOTIPO_H_
# define _PROTOTIPO_H_

# include <stdio.h>
# include <stdlib.h>
# include <stdio.h>
# include <sys/types.h>
# include <sys/ipc.h>
# include <sys/shm.h>
# include <unistd.h>
# include <time.h>

# define TRUE 1
# define FALSE 0
# define MAX 50
# define SystemTime 1


typedef struct proc
{
    int  id;			//ID do processo
    int  chegada;		//Instante de chegada
    int  tempo;			//Tempo de processamento
    int  ioI;			//Instante que inicia o IO
    int  ioT;			//Instante que termina o IO
    int  timer;
    struct   proc *prox;
    struct   proc *ant;
}proc;




void executar(proc *);
proc* LerEntrada(char linha[MAX]);
proc* FCFS(proc *processos, proc *novo);
proc* FCFS_execucao(proc *processos, proc *novo);
proc* RR(proc *processos, proc *novo);
proc* SJF(proc *processos, proc *novo);
void contarTempoES(proc *processos);
proc* empilharES(proc *, proc *);
proc* removelista(proc *lista, proc *pararemover);
//void ES_verifica_saida(proc **pilhaDeES,proc **listaDePronto);
char concatenarSaida(proc *processo);


# endif /*_PROTOTIPO_H_*/