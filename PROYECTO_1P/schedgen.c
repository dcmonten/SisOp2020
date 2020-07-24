#include <stdio.h>
#include <stdlib.h>

void printInstr(){
    printf("\n ************************************************************************************\n");
    printf("\n Generador de procesos de CPU");
    printf("\n Autor: Daniela Montenegro");
    printf("\n Sistemas Operativos 2020-1S\n");
    printf("\n Uso: \n");
    printf("\n ./schedgen 1000 0.1 0.5 schedtimes.dat\n");
    printf("\n ./schedgen [procesos:Integer] [pa:Float] [pb:Float] [filepath:String] \n");
    printf("\na) el total número de procesos N (entero positivo), (b) la probabilidad de llegada de un proceso pa (número real entre 0 y 1), (c) la probabilidad de longitud de ráfaga de un proceso pb (número real entre 0 y 1), y (d) el nombre del archivo que se generará. \n");
    printf("\n ************************************************************************************\n");   
}

int main(int argc, char *argv[])
{
    if (argv[1]==NULL){
        printf ("\n[ERROR] Cantidad de procesos no definida\n");
        printInstr();
        exit(EXIT_FAILURE);
    }
    if(argv[2]==NULL){
        printf ("\n[ERROR] Probabilidad de llegada de proceso no definida\n");
        printInstr();
        exit(EXIT_FAILURE);
    }
    if(argv[3]==NULL){
        printf ("\n[ERROR] Probabilidad de longitud de ráfaga de un proceso no definida\n");
        printInstr();
        exit(EXIT_FAILURE);
    }
    if(argv[4]==NULL){
        printf ("\n[ERROR] Sin nombre de archivo\n");
        printInstr();
        exit(EXIT_FAILURE);
    }
    
    int pcs = strtol(argv[1], NULL, 10);
    float pa = strtof(argv[2], NULL);
    float pb = strtof(argv[3], NULL);

    if (pcs==0){
        printf ("\n[ERROR] Cantidad de procesos debe ser un número mayor a 0\n");
        printInstr();
        exit(EXIT_FAILURE);
    }
    if (pa<0||pb<0){
        printf ("\n[ERROR] Las probabilidades deben ser números mayores o iguales a 0\n");
        printInstr();
        exit(EXIT_FAILURE);
    }
    if (pa>1||pb>1){
        printf ("\n[ERROR] Las probabilidades deben ser números menores o iguales a 1\n");
        printInstr();
        exit(EXIT_FAILURE);
    }
    
    
    FILE *fp;
    fp  = fopen (argv[4], "w");
    float rd1,rd2;

    int ta,tb;
    ta=0;
    tb=1;

    for (int i = 0; i < pcs; i++)
    {
        rd1=rand() / (RAND_MAX + 1.);
        rd2=rand() / (RAND_MAX + 1.);

        if (rd1<pa){
            fprintf(fp, "%d %d\n",ta,tb);
        }
        if (rd2<pb){
            tb=0;
        }
        ta++;
        tb++;
    }
    fclose (fp);
    return 0;
}
