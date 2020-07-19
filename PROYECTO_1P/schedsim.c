#include <stdio.h>
#include<string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

//vars
char scheds_list[3][5] = {"fcfs","sjf","rr"}; //tipos de planificadores
//mutex para acceso al io
pthread_mutex_t mutex_io;
//1. Imprimir ayuda
void printHelp();
//2. Planificador RR
void rr();
//3. Planificador SJF
void sjf();
//4. Planificador FCFS
void fcfs();
//5. Busca el índice del scheduler usando el argumento 1
int indexSched(char* sched);

int main(int argc, char *argv[])
{
    pthread_mutex_init(&mutex_io, NULL);
    //busco el índice del planificador
    int scheduler = indexSched(argv[1]);
    /*
    **  dependiendo del tipo de planificador se aplica una función
    **  si no coincide con ninguno de los planificadores definidos
    **  se imprime la ayuda
    */
    switch (scheduler)
    {
        case 0:
            fcfs();
            break;
        case 1:
            sjf();
            break;
        case 2:
            rr();
            break;
        default:
            printHelp();
            break;
    }
    return 0;
}

void printHelp()
{
    pthread_mutex_lock (&mutex_io);
    printf("\n ************************************************************************************\n");
    printf("\n Simulador de planificador de CPU");
    printf("\n Autor: Daniela Montenegro");
    printf("\n Sistemas Operativos 2020-1S\n");
    printf("\n Uso: \n");
    printf("\n ./schedsim [planificador:String] (opcional: [quantum:Integer] //solo para round robin//)\n");
    printf("\n First Come First Served: ");
    printf("\n ./schedsim fcfs");
    printf("\n Shortest Job First (Apropiativo): ");
    printf("\n ./schedsim sjf");
    printf("\n Round Robin: ");
    printf("\n ./schedsim rr [quantum:Integer]\n");
    printf("\n ************************************************************************************\n");    
    pthread_mutex_unlock (&mutex_io);
    return;
}

int indexSched(char* sched)
{
    for(int i = 0; i < 3; i++)
    { 
        if(strcmp(sched, scheds_list[i]) == 0 )
            return i;
    }
    return -1;
}

void rr(){
    printf("\nRound Robin Scheduler\n");
}
void sjf(){
    printf("\nShortest Job First Scheduler\n");
}
void fcfs(){
    printf("\nFirst Come First Served Scheduler\n");
}