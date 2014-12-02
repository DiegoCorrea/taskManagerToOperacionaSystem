# include <stdlib.h>
# include <stdio.h>
# include <unistd.h>
# include <string.h>

# include "prototipo.h"


int main(int argc, char *argv[])
{
	long pid_algoritmo;
	int tempoTotal, nprocessos = 0;
	float tempoMedio;
	proc *listaDePronto = NULL, *pilhaDeES = NULL, *andante = NULL, *buffer = NULL, *processoNaCPU = NULL;
    char linha[MAX];

	//memoria compartilhada
	int shmid, shmid2;
	key_t key, key2;

	proc *areacritica;
	int *semaforo;

	key = 1420450;
	key2 = 123456;

    FILE *fl_entrada = fopen(argv[2], "r" );

    if ( fl_entrada == 0 || fl_entrada == NULL) {
      fprintf(stderr, "Arquivo %s não encontrado\n", argv[2]);
      exit(1);
    }
    else 
    {
        while (fscanf(fl_entrada, "%s", linha) != EOF) 
        {
        	
            if(strcmp("FCFS",argv[1])==0)
			{
				listaDePronto = FCFS(listaDePronto,LerEntrada(linha));	
			}

            
            if (strcmp("RR",argv[1])==0)
			{
				//Criar algoritmo de ordenação da entrada de acordo com o RR
				listaDePronto = RR(listaDePronto,LerEntrada(linha));
			}
			

			if (strcmp("SJF",argv[1])==0)
			{
				//Criar algoritmo de ordenação da entrada de acordo com o SJF
				//listaDePronto = SJF(listaDePronto,LerEntrada(linha));
			}
			nprocessos +=1;
        }
        fclose( fl_entrada );
    }
    //Neste ponto todos os listaDePronto foram carregados do arquivo e o primeiro processo está no ponteiro *listaDePronto;
    //Que uma lista duplamente encadiada


    //area compartilhada do tamnho do proc
    areacritica = (proc *) malloc(sizeof(proc));
	if ((shmid = shmget(key, sizeof(proc), IPC_CREAT | 0666)) < 0) {
		perror("shmget");
		exit(1);
	}

	if ((areacritica = shmat(shmid, NULL, 0)) == (proc *) -1) {
		perror("shmat");
		exit(1);
	}

	//semaforo para permitir a execução
	semaforo = (int *) malloc(sizeof(int));
	if ((shmid2 = shmget(key2, 1, IPC_CREAT | 0666)) < 0) {
		perror("shmget");
		exit(1);
	}

	if ((semaforo = shmat(shmid2, NULL, 0)) == (int *) -1) {
		perror("shmat");
		exit(1);
	}




	andante = NULL;
	*semaforo = -1;
	tempoTotal = 0;
	pid_algoritmo = fork();
	





	if(pid_algoritmo != 0)
	{
		//Escalonador
		while(1)
		{
			if( *semaforo == 0 || *semaforo == -1 )
			{
				if(*semaforo == 0 && processoNaCPU != NULL)
				{
					processoNaCPU->timer = areacritica->timer;
					
					if (processoNaCPU->timer >= processoNaCPU->tempo)
					{
						FILE *fl_entrada = fopen(argv[3], "a" );

					    if ( fl_entrada == 0 || fl_entrada == NULL) {
					      fprintf(stderr, "Arquivo %s não pode ser escrito\n", argv[3]);
					      exit(1);
					    }
					    //linha = concatenarSaida(processoNaCPU);
					    fprintf(fl_entrada,"%d;%d;%d\n", processoNaCPU->id,processoNaCPU->chegada,tempoTotal);

					    fclose( fl_entrada );

						printf("ESCALONADOR: Processo %d será morto, por tempo de execução ter terminado\n",processoNaCPU->id );
						free(processoNaCPU);
						processoNaCPU = NULL;							
					}
					else
					{
						pilhaDeES = empilharES(pilhaDeES, processoNaCPU);							
						printf("ESCALONADOR: Processo %d empilhado, ficará no IO de: %d até %d\n",processoNaCPU->id, processoNaCPU->ioI,processoNaCPU->ioT );							
					}
				}
				else
				{
					if(*semaforo == -1)
						printf("ESCALONADOR: Entrando pela primeira vez\n");
				}

				if ((listaDePronto != NULL) && (tempoTotal >= listaDePronto->chegada))
				{
					//colocando o primeiro do processo na area critica
					areacritica->id = listaDePronto->id;
					areacritica->tempo = listaDePronto->tempo;
					areacritica->timer = listaDePronto->timer;
					areacritica->ioI = listaDePronto->ioI;
					areacritica->ioT = listaDePronto->ioT;
					areacritica->chegada = listaDePronto->chegada;
					
					processoNaCPU = listaDePronto;
					listaDePronto = listaDePronto->prox;
					processoNaCPU->prox = NULL;
					processoNaCPU->ant = NULL;

					*semaforo = 1;
					printf("\nESCALONADOR: Processo %d escolhido para ir a CPU\n",areacritica->id );
				}
				else
				{
					printf("ESCALONADOR: Nenhum processo disponivel para execucao\n");
				}
			}
			
			if (*semaforo > -1)
			{
				andante = pilhaDeES;
				while(andante != NULL)
			    {
			        if(andante->timer > andante->ioT)
			        {
			        	buffer = andante;
			        	andante = andante->prox;
			            pilhaDeES = removelista(pilhaDeES,buffer);

			            if(strcmp("FCFS",argv[1])==0)
						{
							listaDePronto = FCFS_execucao(listaDePronto,buffer);
							printf("ESCALONADOR: Processo %d saindo do I/O, I/O Final %d, Vida %d\n",buffer->id, buffer->ioT,buffer->timer );	
						}

			            
			            if (strcmp("RR",argv[1])==0)
						{
							//Criar algoritmo de ordenação da entrada de acordo com o RR
							printf ("Você escolheu o RR\n");
							listaDePronto = RR(listaDePronto,buffer);
						}
						

						if (strcmp("SJF",argv[1])==0)
						{
							//Criar algoritmo de ordenação da entrada de acordo com o SJF
							printf ("Você escolheu o SJF\n");
							//listaDePronto = SJF(listaDePronto,buffer);
						}
			        }
			        else
			        	andante = andante->prox;
			    }
				contarTempoES(pilhaDeES);
			}
			tempoTotal += 1;
			sleep(SystemTime);
			printf("\nESCALONADOR: ACODANDO, tempo total é: %d\n", tempoTotal);
			if (listaDePronto == NULL && pilhaDeES == NULL && processoNaCPU == NULL)
			{
				FILE *fl_entrada = fopen(argv[3], "a" );

			    if ( fl_entrada == 0 || fl_entrada == NULL) {
			      fprintf(stderr, "Arquivo %s não pode ser escrito\n", argv[3]);
			      exit(1);
			    }
			    //linha = concatenarSaida(processoNaCPU);
			    fprintf(fl_entrada,"Tempo total: %d\n", tempoTotal);
			    tempoMedio = tempoTotal/nprocessos;
			    fprintf(fl_entrada,"Tempo médio: %f\n", tempoMedio);

			    fclose( fl_entrada );
			    *semaforo = -2;
			    return EXIT_SUCCESS;
			}			
		}
	}
	else
	{
		//CPU
		while(1)
		{
			if ( *semaforo == 1 )
			{
				while( (areacritica->timer < areacritica->tempo) && !((areacritica->timer >= areacritica->ioI) && (areacritica->timer <= areacritica->ioT)) )
				{
					executar(areacritica);
					sleep(SystemTime);
				}
				*semaforo = 0; 
				printf("--CPU: Semaforo desligado\n");
			}
			else
			{
				printf("--CPU idle...\n");
				sleep(SystemTime);
			}
			if(*semaforo == -2)
				return EXIT_SUCCESS;
			
		}
	}

	return EXIT_SUCCESS;
}
