#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#define NUM_THEADS 11


//Estructura que se envía como parámetro
typedef struct {
	int row;
	int column;		
} parameters;

// Sudoku 9x9 resuelto, para que revise todos los casos
int sudoku_resuelto[9][9] = {
	{7, 5, 9, 1, 4, 2, 6, 8, 3},
	{4, 8, 3, 9, 6, 5, 7, 1, 2},
	{6, 1, 2, 3, 7, 8, 9, 4, 5},
	{2, 3, 4, 5, 1, 6, 8, 9, 7},
	{8, 6, 7, 2, 9, 4, 5, 3, 1},
	{5, 9, 1, 8, 3, 7, 4, 2, 6},
	{9, 7, 6, 4, 2, 3, 1, 5, 8},
	{1, 2, 8, 7, 5, 9, 3, 6, 4},
	{1, 4, 5, 6, 8, 1, 2, 7, 9}
};

//declaracion de funciones
//1. Revisr fila
void *revisarFila(void* param);
//2. Revisar columna
void *revisarColumna(void* param);
//3. Revisar 3x3
void *revisarMatriz(void* param);
//4. Duplicados
int duplicado(int* lista);
bool contains(int *values, size_t size, int value);

int main() {	
    pthread_t hilos[NUM_THEADS];
    int index=0;
    int i,j;
    //recorro la matriz 9x9, para los hilos que revisan filas y columnas
    for (i=0;i<9;i++){
        parameters *dt_col = (parameters *) malloc(sizeof(parameters));	
        parameters *dt_row = (parameters *) malloc(sizeof(parameters));	

        dt_col->row = 0;		
		dt_col->column = i;


        dt_row->row = i;		
		dt_row->column = 0;

        
        pthread_create(&hilos[index++],NULL,revisarColumna,dt_col);
        //pthread_create(&hilos[index++],NULL,revisarFila,dt_row);

       
    }

}

void *revisarFila(void* param){
    parameters *params = (parameters*) param;
	int row = params->row;
	int col = params->column;
        //creo una lista donde guardo valores de la columna para revisar duplicados
    int columna[9]={0,0,0,0,0,0,0,0,0};
    //creo columnas
    for (size_t i = 0; i < 9; i++)
    {
        columna[i]=sudoku_resuelto[i][col];   
    }
    int dup=duplicado(columna);
    if (dup>0){
        printf("Duplicado en fila %d columna %d\n", dup+1,col);
        pthread_exit(0); 
    }
    else
    {
        pthread_exit(NULL);
    }
    
}

void *revisarColumna(void* param){
    parameters *params = (parameters*) param;
	int row = params->row;
	int col = params->column;
    //creo una lista donde guardo valores de la columna para revisar duplicados
    int columna[9]={0,0,0,0,0,0,0,0,0};
    //creo columnas
    for (size_t i = 0; i < 9; i++)
    {
        columna[i]=sudoku_resuelto[i][col];   
    }
    int dup=duplicado(columna);
    if (dup>0){
        printf("Duplicado en fila %d columna %d\n", dup+1,col);
        pthread_exit(0); 
    }
    else
    {
        pthread_exit(NULL);
    }
}


//retorno el índice del duplicado
int duplicado(int* lista){
    for (size_t i = 0; i < 9; i++)
    {
        if(contains(lista,i,lista[i])){
            //me salto el valor igual
            continue;
        } 
        //si retorna valor positivo significa que hay duplicado
        if(contains(lista+i+1,8,lista[i])){
            return i;
        }
    }
    //retorna negativo, sin duplicados
    return -1;
}

//revisa si hay duplicados en lista y retorna el estado de verdad
bool contains(int *values, size_t size, int value)
{
    for (size_t i = 0; i < size; ++i)
        if (values[i] == value)
            return true;
    return false;
}