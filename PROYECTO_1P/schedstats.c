#include "sched_basics.h"

void printSchedDescr();

//MAIN
int main(int argc, char *argv[])
{
    if(argv[1]==NULL){
        printf ("\n[ERROR] Sin nombre de archivo\n");
        printSchedDescr();
        exit(EXIT_FAILURE);
    }
    char * file_path = argv[1]; // defino ruta del archivo
    LIST_INIT(&ta_stats);
    LIST_INIT(&tan_stats);
    LIST_INIT(&w_stats);
    LIST_INIT(&brsts);
    for (int i = 0; i < 4; i++)
    {
        if(fillProcessQueues(file_path)){
            switch (i)
            {
            case 0:
                fcfs_silent();
                break;
            case 1:
                sjf_silent();
                break;
            case 2:
                rr_silent(1);
                break;
            case 3: 
                rr_silent(4);
                break;
            default:
                break;
            }
        }
        else{
            printf ("\n[ERROR] Error con el archivo. Asegurese de haber escrito correctamente la ruta\n");
            printSchedDescr();
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}

void printSchedDescr()
{
    printf("\n ************************************************************************************\n");
    printf("\n Estadísticas de planificadores de CPU");
    printf("\n Autor: Daniela Montenegro");
    printf("\n Sistemas Operativos 2020-1S\n");
    printf("\n Uso: \n");
    printf("\n ./schedstat [schedtimes.dat - path:String]\n");
    printf("\n ************************************************************************************\n");    
    return;
}


