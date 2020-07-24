#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include <limits.h>
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

typedef struct _FileStats {
    LIST_ENTRY(_FileStats) pointers;
    int id;
    int burst;
    float wait;
    float turnaround;
    float norm_turnaround;
    int procs;
} FileStats;


//LISTAS ENLAZADAS
LIST_HEAD(process_list, _Process) processes;
LIST_HEAD(process_stats, _ProcessStats) processes_stats;
LIST_HEAD(fcfs_list, _FileStats) fcfs_f;
LIST_HEAD(sjf_list, _FileStats) sjf_f;
LIST_HEAD(rr1_list, _FileStats) rr1_f;
LIST_HEAD(rr4_list, _FileStats) rr4_f;


//INDEXACIÓN DE SCHEDULERS
int indexSched(char* sched);
//OPERACIONES BÁSICAS
int remainingTime(int cputime, int burst);
int turnaroundTime(int exit, int arrival);
int waitingTime(int turnar,int burst);
int burst(int turnar, int wait);
//CREACIÓN DE STRUCTS
Process *create_process(int id,int arrival,int burst);
Process * shortestJob(Process * ready,int arrival);
ProcessStats *create_process_stats(int id,int turnaround,int wait);
FileStats *create_file_stats(int id,int burst, float wait,float turnaround,float norm_turnaround);


//AUXILIARES DE PROCESOS
int executeProcess(Process * ready, int cputime,int arrival_time);
bool removeExecutedProcess(Process * executed);
int sjfNextStop(Process * ready,int arrival);

//LENADO DE LISTA
bool fillProcessQueues(char * file_path);

//ESTADÍSTICAS
void runStatsSilentFCFS();
void printFileStatsFCFS();
void runStatsSilentSJF();
void printFileStatsSJF();
void runStatsSilentRR1();
void printFileStatsRR1();
void runStatsSilentRR4();
void printFileStatsRR4();
void listsToFiles();
void runStats(int end);
void freeStats();
//SCHEDULERS
void rr(long quantum);
void sjf();
void fcfs();
//SILENT SCHEDULERS
void rr_silent(long quantum);
void sjf_silent();
void fcfs_silent();