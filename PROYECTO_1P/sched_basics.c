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

FileStats *create_file_stats(int id,int burst, float wait,float turnaround,float norm_turnaround){
    FileStats *fs = (FileStats *)malloc(sizeof(FileStats));
    fs->id=id;
    fs->burst=burst;
    fs->wait=wait;
    fs->turnaround=turnaround;
    fs->norm_turnaround=norm_turnaround;
    fs->procs=0;
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
        //ejecuto los procesos en orden de llegada, ls doy tiempo de cpu equivalente al quatum definido
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
    //lo mismo que el anterior solo que este no imprime en consola
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
//Reduce el burst actual del proceso, para poder descartarlo cuando legue a 0
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
//Descarta los procesos cuyo burst restante es 0 
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
/*SJF sin salida de consola*/
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
    runStatsSilentSJF();
}
/*SJF itera cada item en busca del que tiene el menor tiempo restante y lo ejecuta, siempre empieza desde el primer item*/
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
        int step=sjfNextStop(curr, start);//el next stop le dice al sjf cuanto debe durar el burst
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
//REtorna el proceso con menor duración de toda la lista de proceso sque ya hayan llegado.
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


//Ejecuta el primer proceso que llega, por completo.
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
//Lo mismo que el anterios, pero no imprime la salida de la pantallla
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
    runStatsSilentFCFS();
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

//Revisa la lista de estadísticas e imprime las estadñisticas globales de la ejecución
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
//Revisa la lista de estadísticas y guarda las estadisticas ordenadas por burst en la lista de stats de fcfs

void runStatsSilentFCFS(){
    LIST_INIT(&fcfs_f);
    int bs_min = INT_MAX;
    int bs_max = 0;
    ProcessStats *pst;
    //obtengo el burst minimo y el burst maximo
    LIST_FOREACH(pst, &processes_stats, pointers) {
        int bs=burst(pst->turnaround,pst->wait);
        if (bs>bs_max){
            bs_max=bs;
        }
        if (bs<bs_min){
            bs_min=bs;
        }
    }
    FileStats *fa;
    //por cada valor de burst
    for (int i = bs_min; i <= bs_max; i++)
    {
        fa = create_file_stats(0,i,0.0f,0.0f,0.0f);
        ProcessStats *ps;
        //busco en toda la lista de procesos
        LIST_FOREACH(ps, &processes_stats, pointers) {
            int bs=burst(ps->turnaround,ps->wait);
            if(bs==i){
                //acumulo en wait y turnaround
                fa->wait=fa->wait+(float)ps->wait;
                fa->turnaround=fa->turnaround+(float)ps->turnaround;
                fa->norm_turnaround=(float)ps->turnaround/(float)bs;
                fa->procs=fa->procs+1;
            }else continue;
        }
        if(fa->procs>0){
            //la acumulaci{on la cambio antes de insertar}
            fa->wait=fa->wait/(float)fa->procs;
            fa->turnaround=fa->turnaround/(float)fa->procs;
            LIST_INSERT_HEAD(&fcfs_f, fa, pointers);
        }  
        else continue;
    }
}
//Revisa la lista de estadísticas y guarda las estadisticas ordenadas por burst en la lista de stats de sjf

void runStatsSilentSJF(){
    LIST_INIT(&sjf_f);
    int bs_min = INT_MAX;
    int bs_max = 0;
    ProcessStats *pst;
    //obtengo el burst minimo y el burst maximo
    LIST_FOREACH(pst, &processes_stats, pointers) {
        int bs=burst(pst->turnaround,pst->wait);
        if (bs>bs_max){
            bs_max=bs;
        }
        if (bs<bs_min){
            bs_min=bs;
        }
    }
    FileStats *fa;
    //por cada valor de burst
    for (int i = bs_min; i <= bs_max; i++)
    {
        fa = create_file_stats(0,i,0.0f,0.0f,0.0f);
        ProcessStats *ps;
        //busco en toda la lista de procesos
        LIST_FOREACH(ps, &processes_stats, pointers) {
            int bs=burst(ps->turnaround,ps->wait);
            if(bs==i){
                //acumulo en wait y turnaround
                fa->wait=fa->wait+(float)ps->wait;
                fa->turnaround=fa->turnaround+(float)ps->turnaround;
                fa->norm_turnaround=(float)ps->turnaround/(float)bs;
                fa->procs=fa->procs+1;
            }else continue;
        }
        if(fa->procs>0){
            //la acumulaci{on la cambio antes de insertar}
            fa->wait=fa->wait/(float)fa->procs;
            fa->turnaround=fa->turnaround/(float)fa->procs;
            LIST_INSERT_HEAD(&sjf_f, fa, pointers);
            //printf ("\n%d %.2f %.2f %.2f %d\n",fa->burst,fa->turnaround,fa->wait,fa->norm_turnaround,fa->procs);

        }  
        else continue;
    }
}
//Revisa la lista de estadísticas y guarda las estadisticas ordenadas por burst en la lista de stats de rr con q=1

void runStatsSilentRR1(){
    LIST_INIT(&rr1_f);
    int bs_min = INT_MAX;
    int bs_max = 0;
    ProcessStats *pst;
    //obtengo el burst minimo y el burst maximo
    LIST_FOREACH(pst, &processes_stats, pointers) {
        int bs=burst(pst->turnaround,pst->wait);
        if (bs>bs_max){
            bs_max=bs;
        }
        if (bs<bs_min){
            bs_min=bs;
        }
    }
    FileStats *fa;
    //por cada valor de burst
    for (int i = bs_min; i <= bs_max; i++)
    {
        fa = create_file_stats(0,i,0.0f,0.0f,0.0f);
        ProcessStats *ps;
        //busco en toda la lista de procesos
        LIST_FOREACH(ps, &processes_stats, pointers) {
            int bs=burst(ps->turnaround,ps->wait);
            if(bs==i){
                //acumulo en wait y turnaround
                fa->wait=fa->wait+(float)ps->wait;
                fa->turnaround=fa->turnaround+(float)ps->turnaround;
                fa->norm_turnaround=(float)ps->turnaround/(float)bs;
                fa->procs=fa->procs+1;
            }else continue;
        }
        if(fa->procs>0){
            //la acumulaci{on la cambio antes de insertar}
            fa->wait=fa->wait/(float)fa->procs;
            fa->turnaround=fa->turnaround/(float)fa->procs;
            LIST_INSERT_HEAD(&rr1_f, fa, pointers);
            //printf ("\n%d %.2f %.2f %.2f %d\n",fa->burst,fa->turnaround,fa->wait,fa->norm_turnaround,fa->procs);

        }  
        else continue;
    }
}
//Revisa la lista de estadísticas y guarda las estadisticas ordenadas por burst en la lista de stats de rr con q=4

void runStatsSilentRR4(){
    LIST_INIT(&rr4_f);
    int bs_min = INT_MAX;
    int bs_max = 0;
    ProcessStats *pst;
    //obtengo el burst minimo y el burst maximo
    LIST_FOREACH(pst, &processes_stats, pointers) {
        int bs=burst(pst->turnaround,pst->wait);
        if (bs>bs_max){
            bs_max=bs;
        }
        if (bs<bs_min){
            bs_min=bs;
        }
    }
    FileStats *fa;
    //por cada valor de burst
    for (int i = bs_min; i <= bs_max; i++)
    {
        fa = create_file_stats(0,i,0.0f,0.0f,0.0f);
        ProcessStats *ps;
        //busco en toda la lista de procesos
        LIST_FOREACH(ps, &processes_stats, pointers) {
            int bs=burst(ps->turnaround,ps->wait);
            if(bs==i){
                //acumulo en wait y turnaround
                fa->wait=fa->wait+(float)ps->wait;
                fa->turnaround=fa->turnaround+(float)ps->turnaround;
                fa->norm_turnaround=(float)ps->turnaround/(float)bs;
                fa->procs=fa->procs+1;
            }else continue;
        }
        if(fa->procs>0){
            //la acumulaci{on la cambio antes de insertar}
            fa->wait=fa->wait/(float)fa->procs;
            fa->turnaround=fa->turnaround/(float)fa->procs;
            LIST_INSERT_HEAD(&rr4_f, fa, pointers);
            //printf ("\n%d %.2f %.2f %.2f %d\n",fa->burst,fa->turnaround,fa->wait,fa->norm_turnaround,fa->procs);

        }  
        else continue;
    }
    
}

//Revisa la lista de estadísticas por tipo de planificador y la guarda en archivos

void listsToFiles(){
    FILE *schedtur;
    FILE *schedntur;
    FILE *schedw;

    schedtur  = fopen ("schedturns.dat", "w");
    schedntur  = fopen ("schednturns.dat", "w");
    schedw  = fopen ("schedwaits.dat", "w");


    int bs_min = INT_MAX;
    int bs_max = 0;
    FileStats *fs;
    //obtengo el burst minimo y el burst maximo
    LIST_FOREACH(fs, &fcfs_f, pointers) {
        int bs=fs->burst;
        if (bs>bs_max){
            bs_max=bs;
        }
        if (bs<bs_min){
            bs_min=bs;
        }
    }
    //por cada valor de burst
    for (int i = bs_min; i <= bs_max; i++)
    {
        int burst = i;
        float turnar_f,turnar_s,turnar_r1,turnar_r4;
        float wt_f,wt_s,wt_r1,wt_r4;
        float nturnar_f,nturnar_s,nturnar_r1,nturnar_r4;
        FileStats *fc,*fsj,*fr1,*fr4;
        int index=0;
        LIST_FOREACH(fc, &fcfs_f, pointers) {
            if(fc->burst == burst){
                turnar_f=fc->turnaround;
                wt_f=fc->wait;
                nturnar_f=fc->norm_turnaround;
                index++;
            }
        }
        LIST_FOREACH(fsj, &sjf_f, pointers) {
            if(fsj->burst == burst){
                turnar_s=fsj->turnaround;
                wt_s=fsj->wait;
                nturnar_s=fsj->norm_turnaround;
                index++;
            }         
        }
        LIST_FOREACH(fr1, &rr1_f, pointers) {
            if(fr1->burst == burst){
                turnar_r1=fr1->turnaround;
                wt_r1=fr1->wait;
                nturnar_r1=fr1->norm_turnaround;
                index++;
            }             
        }
        LIST_FOREACH(fr4, &rr4_f, pointers) {
            if(fr4->burst == burst){
                turnar_r4=fr4->turnaround;
                wt_r4=fr4->wait;
                nturnar_r4=fr4->norm_turnaround;
                index++;
            }           
        }
        if(index==4) {
            fprintf(schedtur,"\n%d %.2f %.2f %.2f %.2f\n",burst,turnar_f,turnar_s,turnar_r1,turnar_r4);
            fprintf (schedw,"\n%d %.2f %.2f %.2f %.2f\n",burst,wt_f,wt_s,wt_r1,wt_r4);
            fprintf (schedntur,"\n%d %.2f %.2f %.2f %.2f\n",burst,nturnar_f,nturnar_s,nturnar_r1,nturnar_r4);
        }


    }
     fclose (schedtur);  
     fclose (schedntur);   
     fclose (schedw);   
    }

//libera la memoria de la lista de estadísticas de procesos.
void freeStats(){
    ProcessStats *node;
    while (!LIST_EMPTY(&processes_stats)) {
        node = LIST_FIRST(&processes_stats);
        LIST_REMOVE(node, pointers);
        free(node);
    }
}


