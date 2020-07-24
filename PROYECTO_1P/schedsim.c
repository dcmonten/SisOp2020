#include "sched_basics.h"

//1. Imprimir ayuda
void printHelp();

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
    return;
}
