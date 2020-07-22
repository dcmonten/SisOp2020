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
    int exec_start;
    int exec_end;
    int burst_init;
    int burst;
    int id;
} Process;

typedef struct _ProcessStats {
    LIST_ENTRY(_ProcessStats) pointers;
    int turnaround;
    int wait;
    int id;
} ProcessStats;

Process *create_process(int id,int arrival,int burst);
ProcessStats *create_process_stats(int id,int turnaround,int wait);

LIST_HEAD(process_list, _Process) processes;
LIST_HEAD(process_stats, _ProcessStats) processes_stats;

//vars
char scheds_list[3][5] = {"fcfs","sjf","rr"}; //tipos de planificadores
//mutex para acceso al io
pthread_mutex_t mutex_io;
//1. Imprimir ayuda
void printHelp();
//2. Planificador RR y funciones auxiliares de rr
int remainingTime(int cputime, int burst);
int waitingTime(int turnar,int burst);
int turnaroundTime(int exit, int arrival);
int executeProcess(Process * ready, int cputime, int arrival_time);
bool removeExecutedProcess(Process * executed);
void runStats(int end);
void freeStats();
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

    printf("\n[INFO] SCHEDULER: Round Robin Scheduler\n");
    printf("\n[INFO] Quantum: %ld\n",quantum);
    int time_now=0;
    int skipped=0;
    int index = 1;
    Process *curr;
    while (!LIST_EMPTY(&processes)) {
        skipped=0;
        Process *curr;
        int start,end,burst,turnaround,wait;
        LIST_FOREACH(curr, &processes, pointers) {
            if(curr->arrival>time_now)
            {
                skipped++;
            }
            else
            {
                start = time_now;
                int adt=executeProcess(curr,quantum,time_now);
                time_now=time_now+adt; 
                end=time_now;
                burst=end-start;
                turnaround=end-curr->arrival;
                wait=waitingTime(turnaround,curr->burst_init-curr->burst);
                printf("\n%d: runs %d-%d -> end = %d, (arr = %d), turn = %d, (burst = %d), wait = %d\n",
                        index,start,end,end,curr->exec_start,turnaround,burst,wait
                    );
                index++;
                removeExecutedProcess(curr);
            }
        } 
        if (skipped>0){
            int time=time_now;
            time_now++;
            printf("\n%d: runs %d-%d\n",index,time,time_now);
            index++;
        }
    }
    runStats(time_now);


}
//rr aux
int remainingTime(int cputime, int burst){
    return burst-cputime;
}
int turnaroundTime(int exit, int arrival){
    return exit - arrival;
}
int waitingTime(int turnar,int burst){
    return turnar - burst;
}
int executeProcess(Process * ready, int cputime,int arrival_time){
    int rem=remainingTime(cputime,ready->burst);
    int time=0;
    ready->exec_start = arrival_time;
    if (rem>0){
        ready->exec_end = arrival_time+cputime;
        ready->burst = rem;
        time=cputime;
    }else {
        ready->exec_end = arrival_time+ready->burst;
        time=ready->burst;
        ready->burst = 0;
    }
    return time;
}

bool removeExecutedProcess(Process * executed){
    if(executed->burst == 0){
        int tat=turnaroundTime(executed->exec_end,executed->arrival);
        int wt=waitingTime(tat,executed->burst_init);
        ProcessStats * ps = create_process_stats(executed->id,tat,wt);
        LIST_INSERT_HEAD(&processes_stats, ps, pointers);
        LIST_REMOVE(executed, pointers);
        return true; 
    }
    else return false;
}

void runStats(int end){
    ProcessStats *ps;
    int turnaround_sum=0;
    int wait_sum=0;
    float normal_turnaround_sum=0.0f;
    int procs=0;
    LIST_FOREACH(ps, &processes_stats, pointers) {
        turnaround_sum=turnaround_sum+ps->turnaround;
        wait_sum=wait_sum+ps->wait;
        procs++;
        float bt=(float)ps->turnaround-(float)ps->wait;
        float nm_t=(float)ps->turnaround/bt;
        normal_turnaround_sum=normal_turnaround_sum+nm_t;
    }
    float avg_tat=(float)turnaround_sum/(float)procs;
    float avg_wt=(float)wait_sum/(float)procs;
    float avg_tat_n=normal_turnaround_sum/(float)procs;
    printf("\nEnded at %d time units\n",end);
    printf("\nAverage Turnaround Time: %f time units\n",avg_tat);
    printf("\nAverage Turnaround Time (Normalized): %f time units\n",avg_tat_n);
    printf("\nAverage Wait Time: %f time units\n",avg_wt);
    freeStats();
}

void freeStats(){
    ProcessStats *node;
    while (!LIST_EMPTY(&processes_stats)) {
        node = LIST_FIRST(&processes_stats);
        LIST_REMOVE(node, pointers);
        free(node);
    }
}

/*SJF*/
void sjf(){
    printf("\nShortest Job First Scheduler\n");
}
void fcfs(){   
    printf("\n[INFO] SCHEDULER: First Come First Serve Scheduler\n");
    int time_now=0;
    int skipped=0;
    int index = 1;
    Process *curr;
    while (!LIST_EMPTY(&processes)) {
        skipped=0;
        Process *curr;
        int start,end,burst,turnaround,wait;
        LIST_FOREACH(curr, &processes, pointers) {
            if(curr->arrival>time_now)
            {
                skipped++;
            }
            else
            {
                start = time_now;
                int adt=executeProcess(curr,curr->burst_init,time_now);
                time_now=time_now+adt; 
                end=time_now;
                burst=end-start;
                turnaround=end-curr->arrival;
                wait=waitingTime(turnaround,curr->burst_init-curr->burst);
                printf("\n%d: runs %d-%d -> end = %d, (arr = %d), turn = %d, (burst = %d), wait = %d\n",
                        index,start,end,end,curr->exec_start,turnaround,burst,wait
                    );
                index++;
                removeExecutedProcess(curr);
            }
        } 
        if (skipped>0){
            int time=time_now;
            time_now++;
            printf("\n%d: runs %d-%d\n",index,time,time_now);
            index++;
        }
    }
    runStats(time_now);
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
    LIST_INIT(&processes_stats);
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
        //printf("\n[INFO] Leyendo procesos desde archivo... Proceso #%d -- Llegada: %d Ráfaga: %d\n",index,llegada,rafaga); //mensaje informativo
    } 
    fclose(fp);
    return true;
}
Process *create_process(int id,int arrival,int burst)
{
    Process *process = (Process *)malloc(sizeof(Process));
    process->id = id;
    process->exec_start = 0;
    process->exec_end = 0;
    process->arrival = arrival;
    process->burst = burst;
    process->burst_init = burst;
}

ProcessStats *create_process_stats(int id,int turnaround,int wait){
    ProcessStats *process_stats = (ProcessStats *)malloc(sizeof(ProcessStats));
    process_stats->id = id;
    process_stats->turnaround = turnaround;
    process_stats->wait = wait;
}