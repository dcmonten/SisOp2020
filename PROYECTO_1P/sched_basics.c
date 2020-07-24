#include "sched_basics.h"
int indexSched(char* sched)
{
    char scheds_list[3][5]={"fcfs","sjf","rr"};
    for(int i = 0; i < 3; i++)
    { 
        if(strcmp(sched, scheds_list[i]) == 0 )
            return i;
    }
    return -1;
}
int remainingTime(int cputime, int burst){
    return burst-cputime;
}
int turnaroundTime(int exit, int arrival){
    return exit - arrival;
}
int waitingTime(int turnar,int burst){
    return turnar - burst;
}
int burst(int turnar, int wait){
    return turnar-wait;
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

FileStats *create_file_stats(int burst){
    FileStats *fs = (FileStats *)malloc(sizeof(FileStats));
    fs->burst = burst; 
    fs->fcfs = 0;
    fs->sjf = 0;
    fs->rr1 =0;
    fs->rr4 =0;
}
//ROUND ROBIN
void rr(long quantum){

    printf("\n[INFO] SCHEDULER: Round Robin Scheduler\n");
    printf("\n[INFO] Quantum: %ld\n",quantum);
    int time_now=0;
    int index = 1;
    while (!LIST_EMPTY(&processes)) {
        int time_init = 0;
        Process *curr;
        int start,end,burst,turnaround,wait;
        LIST_FOREACH(curr, &processes, pointers) {
            if(curr->arrival<=time_now)
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
                time_init=end;
                removeExecutedProcess(curr);
            }
            else continue;
        } 
        if (time_init==0){
            int time=time_now;
            time_now++;
            printf("\n%d: runs %d-%d\n",index,time,time_now);
            index++;
        }
    }
    runStats(time_now);


}
//ROUND ROBIN SILENT
void rr_silent(long quantum){

    printf("\n[INFO] Processing Round Robin Scheduler\n");
    printf("\n[INFO] Quantum: %ld\n",quantum);
    int time_now=0;
    int index = 1;
    while (!LIST_EMPTY(&processes)) {
        int time_init = 0;
        Process *curr;
        LIST_FOREACH(curr, &processes, pointers) {
            if(curr->arrival<=time_now)
            {
                int adt=executeProcess(curr,quantum,time_now);
                time_now=time_now+adt; 
                index++;
                time_init=time_now;
                removeExecutedProcess(curr);
            }
            else continue;
        } 
        if (time_init==0){
            int time=time_now;
            time_now++;
            index++;
        }
    }
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
/*SJF*/
void sjf_silent(){
    printf("\n[INFO] Processing Shortest Job First Scheduler Scheduler\n");
    int time_now=0;
    int skipped=0;
    int index = 1;
    while (!LIST_EMPTY(&processes)) {
        Process *curr = LIST_FIRST(&processes);
        int start;
        start = time_now;
        curr = shortestJob(curr,start); 
        int step=sjfNextStop(curr, start);
        int expected_brst=(step<curr->burst)?step:curr->burst;
        if (curr->arrival<=time_now){
            int adt=executeProcess(curr,expected_brst,time_now);
            time_now=time_now+adt; 
            removeExecutedProcess(curr);
        }else{
            int time=time_now;
            time_now++;
        }
        index++;
    }
   
}
/*SJF*/
void sjf(){
    printf("\n[INFO] SCHEDULER: Shortest Job First Scheduler Scheduler\n");
    int time_now=0;
    int skipped=0;
    int index = 1;
    while (!LIST_EMPTY(&processes)) {
        Process *curr = LIST_FIRST(&processes);
        int start,end,burst,turnaround,wait;
        start = time_now;
        curr = shortestJob(curr,start); 
        int step=sjfNextStop(curr, start);
        int expected_brst=(step<curr->burst)?step:curr->burst;
        if (curr->arrival<=time_now){
            int adt=executeProcess(curr,expected_brst,time_now);
            time_now=time_now+adt; 
            end=time_now;
            burst=end-start;
            turnaround=end-curr->arrival;
            wait=waitingTime(turnaround,curr->burst_init-curr->burst);
            printf("\n%d: runs %d-%d -> end = %d, (arr = %d), turn = %d, (burst = %d), wait = %d\n",
                    index,start,end,end,curr->exec_start,turnaround,burst,wait
                );
            
            removeExecutedProcess(curr);
        }else{
            int time=time_now;
            time_now++;
            printf("\n%d: runs %d-%d\n",index,time,time_now);
        }
        index++;
        
    }
    runStats(time_now);
}
int sjfNextStop(Process * ready,int arrival){
    Process * ready_on_q;
    int next_arrival=10000;
    LIST_FOREACH(ready_on_q, &processes, pointers) {
        if(ready_on_q->arrival>arrival)
            next_arrival=(ready_on_q->arrival<next_arrival)?ready_on_q->arrival:next_arrival;
    }
    return (next_arrival<ready->burst) ? next_arrival:ready->burst;
}
Process * shortestJob(Process * ready,int arrival){
    Process * ready_on_q;
    Process * tmp = ready;
    LIST_FOREACH(ready_on_q, &processes, pointers) {
        if(ready_on_q->arrival<=arrival)
        {
            int b1=tmp->burst;
            int b2=ready_on_q->burst;
            if(b1>b2){
                tmp=ready_on_q;
            }
        }
    }
    return tmp;
}



void fcfs(){   
    printf("\n[INFO] SCHEDULER: First Come First Serve Scheduler\n");
    int time_now=0;
    int index = 1;
    while (!LIST_EMPTY(&processes)) {
        int time_init=0;
        Process *curr;
        int start,end,burst,turnaround,wait;
        LIST_FOREACH(curr, &processes, pointers) {
            if(curr->arrival<=time_now)
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
                time_init=end;
                index++;
                removeExecutedProcess(curr);
            }
        } 
        if (time_init==0){
            int time=time_now;
            time_now++;
            printf("\n%d: runs %d-%d\n",index,time,time_now);
            index++;
        }
    }
    runStats(time_now);
}
void fcfs_silent(){   
    printf("\n[INFO] Processing First Come First Serve Scheduler\n");
    int time_now=0;
    int index = 1;
    while (!LIST_EMPTY(&processes)) {
        int time_init=0;
        Process *curr;
        int start;
        LIST_FOREACH(curr, &processes, pointers) {
            if(curr->arrival<=time_now)
            {
                int adt=executeProcess(curr,curr->burst_init,time_now);
                time_now=time_now+adt; 
                time_init=time_now;
                index++;
                removeExecutedProcess(curr);
            }
        } 
        if (time_init==0){
            int time=time_now;
            time_now++;
            index++;
        }
    }
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


/***Stats*****/

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
    printf("\nAverage Turnaround Time: %.2f time units\n",avg_tat);
    printf("\nAverage Turnaround Time (Normalized): %.2f time units\n",avg_tat_n);
    printf("\nAverage Wait Time: %.2f time units\n",avg_wt);
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

