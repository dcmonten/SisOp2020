#include <stdio.h>
#include<string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <ctype.h>
#include "libs/queue.h"

//structs
typedef struct _Process {
    LIST_ENTRY(_Process) pointers;
    int arrival;
    int burst;
    int id;
} Process;
Process *create_process(int id,int arrival,int burst);
LIST_HEAD(process_list, _Process) processes;
//vars
char scheds_list[3][5] = {"fcfs","sjf","rr"}; //tipos de planificadores
//mutex para acceso al io
pthread_mutex_t mutex_io;
//1. Imprimir ayuda
void printHelp();
//2. Planificador RR y funciones auxiliares de rr
int remainingTime(int cputime, int burst);
int executeProcess(Process * ready, int cputime);
bool removeExecutedProcess(Process * executed);

void rr(long quantum);
//3. Planificador SJF
void sjf();
//4. Planificador FCFS
void fcfs();
//5. Busca el índice del scheduler usando el argumento 1
int indexSched(char* sched);
//6. llena las listas de arrival y burst con el archivo, retorna true si lo hizo satisfactoriamente,
//false cuando la ruta del archivo es incorrecta.
bool fillProcessQueues(char * file_path);

//MAIN
int main(int argc, char *argv[])
{
    long q;
    //Verifico que se hayan ingresado correctamente los argumentos al comando, caso contrario, no hago ni guardo nada.
    if (argv[1]==NULL){
        printf ("\n[ERROR] Archivo no definido\n");
        printHelp();
        exit(EXIT_FAILURE);
    }
    if(argv[2]==NULL){
        printf ("\n[ERROR] Planificador no definido\n");
        printHelp();
        exit(EXIT_FAILURE);
    }
    if(strcmp("rr", argv[2]) == 0){
        if(argv[3]==NULL){
            printf ("\n[ERROR] No se definió se definió el quantum para round robin.\n");
            printHelp();
            exit(EXIT_FAILURE);
        }
        else{
            char *ptr;
            q = strtol(argv[3], &ptr, 10);
            if(q<=0){
                printf ("\n[ERROR] Quantum definido en argumento no contiene un número. Cambielo por un número y vuelva a intentar.\n");
                printHelp();
                exit(EXIT_FAILURE);
            }
        }
    }
    //aqui si empiezo a definir variables
    pthread_mutex_init(&mutex_io, NULL); // defino mutex para io
    char * file_path = argv[1]; // defino ruta del archivo
    int scheduler = indexSched(argv[2]); //tipo de sched//busco el índice del planificador
    if(fillProcessQueues(file_path)){
        /*
        **  Dependiendo del tipo de planificador se aplica una función
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
                rr(q);
                break;
            default:
                printHelp();
                break;
        }
    }
    else{
        printf ("\n[ERROR] Error con el archivo. Asegurese de haber escrito correctamente la ruta\n");
        printHelp();
        exit(EXIT_FAILURE);
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
    printf("\n ./schedsim [schedtimes.dat - path:String] [planificador:String] (opcional: [quantum:Integer] //solo para round robin//)\n");
    printf("\n First Come First Served: ");
    printf("\n ./schedsim schedtimes.dat fcfs");
    printf("\n Shortest Job First (Apropiativo): ");
    printf("\n ./schedsim schedtimes.dat sjf");
    printf("\n Round Robin: ");
    printf("\n ./schedsim schedtimes.dat rr [quantum:Integer]\n");
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
void rr(long quantum){
    printf("\nRound Robin Scheduler\n");
    printf("\nQuantum: %ld\n",quantum);
    //LIST_HEAD(rr_ready, _Process) rr_ready;
    //LIST_HEAD(rr_execute, _Process) rr_execute;
    //Process *p;
    //LIST_INSERT_HEAD(&processes, p, pointers);
    
    int time_now=0;
    Process *curr;
    while (!LIST_EMPTY(&processes)) {
        Process *curr;
        LIST_FOREACH(curr, &processes, pointers) {
            printf("\n[INFO] EXECUTE # %d, Arr: %d, Brts: %d\n", curr->id,curr->arrival,curr->burst);  
            time_now=time_now+executeProcess(curr,quantum); 
            if(removeExecutedProcess(curr)){
                printf("\n[INFO] TERMINATE # %d, Arr: %d, Brts: %d\n", curr->id,curr->arrival,curr->burst);  
            }else{
                printf("\n[INFO] TO READY # %d, Arr: %d, Brts: %d\n", curr->id,curr->arrival,curr->burst);  
            }
        } 
    }
    printf("\nEnded at %d time units\n",time_now);
}
//rr aux
int remainingTime(int cputime, int burst){
    return burst-cputime;
}

int executeProcess(Process * ready, int cputime){
    int rem=remainingTime(cputime,ready->burst);
    int time=0;
    if (rem>0){
        ready->arrival = ready->arrival+cputime;
        ready->burst = rem;
        time=time+cputime;
    }else {
        time=time+ready->burst;
        ready->arrival = ready->arrival+ready->burst;
        ready->burst = 0;
    }
    return time;
}
bool removeExecutedProcess(Process * executed){
    if(executed->burst == 0){
        LIST_REMOVE(executed, pointers);
        return true; 
    }
    else return false;
}


/*SJF*/
void sjf(){
    printf("\nShortest Job First Scheduler\n");
}
void fcfs(){
    printf("\nFirst Come First Served Scheduler\n");
}
bool fillProcessQueues(char * file_path){
    int llegada,rafaga;//variables temporales para almacenar los digitos del archivo cuando se lea linea por linea
    int index=0;//número de línea
    FILE *fp; //se define el archivo con la data de procesos 
    fp = fopen(file_path, "r");//archivo
    if (fp == NULL){
        return false;
    }
    LIST_INIT(&processes);
    Process * prev;//temporal para guardar el elemento anterior
    while(fscanf(fp, "%d %d", &llegada, &rafaga)!= EOF){
        index++;
        Process *p = create_process(index,llegada,rafaga);
        if(index==1){
            LIST_INSERT_HEAD(&processes, p, pointers);
        }
        else {
            LIST_INSERT_AFTER(prev, p, pointers);
        }
        prev=p;
        printf("\n[INFO] Leyendo procesos desde archivo... Proceso #%d -- Llegada: %d Ráfaga: %d\n",index,llegada,rafaga); //mensaje informativo
    } 
    fclose(fp);
    return true;
}
Process *create_process(int id,int arrival,int burst)
{
    Process *process = (Process *)malloc(sizeof(Process));
    process->id = id;
    process->arrival = arrival;
    process->burst = burst;
}