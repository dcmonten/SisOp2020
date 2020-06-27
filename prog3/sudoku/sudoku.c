#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include<signal.h> 

#define NUM_THEADS 27
#define SIGQUIT 3 
//Estructura que se envía como parámetro
typedef struct {
	int row;
	int column;		
} parameters;
//mutex para escribir sobre la varible dups
pthread_mutex_t mutexsum;

int dups = 0;

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
	{3, 4, 5, 6, 8, 1, 2, 7, 9}
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
    //mutex para aumentar el numero de duplicados correctamente en dups
    pthread_mutex_init(&mutexsum, NULL);             
    
    for (i=0;i<9;i++){
        //creo los hilos con su parametro y varío el indice para leer filas y columnas
        parameters *dt_col = (parameters *) malloc(sizeof(parameters));	
        dt_col->row = 0;		
		dt_col->column = i;
        pthread_create(&hilos[index++],NULL,revisarColumna,dt_col);

        parameters *dt_row = (parameters *) malloc(sizeof(parameters));	
        dt_row->row = i;		
		dt_row->column = 0;
        pthread_create(&hilos[index++],NULL,revisarFila,dt_row);
        //necesito otro for para enviar los datos de las matrices
        for (int j = 0; j < 9; j++)
        {
            //solo necesito hacer cortes en índices multiplos de 3
            if(j%3==0 && i%3==0){
                parameters *dt_matrix = (parameters *) malloc(sizeof(parameters));	
                dt_matrix->row = i;		
                dt_matrix->column = j;
                pthread_create(&hilos[index++],NULL,revisarMatriz,dt_matrix);
            }

        }
        
    }

	for (i = 0; i < NUM_THEADS; i++) {
		pthread_join(hilos[i], NULL);			// Wait for all threads to finish
	}
    
    if (dups>0)
    {
        printf("Sudoku con duplicados\n");
    }
    else
    {
        printf("Sudoku resuelto correctamente\n");
    }
    
    pthread_mutex_destroy(&mutexsum);
	return EXIT_SUCCESS;

}

//valida fila
void *revisarFila(void* param){
    parameters *params = (parameters*) param;
	int row = params->row;
    int col=params->column; 
    //tomo la fila
    int fila[9]={0,0,0,0,0,0,0,0,0};
    //creo columnas
    for (size_t i = 0; i < 9; i++)
    {
        fila[i]=sudoku_resuelto[row][i];   
    }
    //busco duplicados en la fila
    int dup=duplicado(fila);
    if (dup>0){
        printf("Numero duplicado en fila %d\n",row);
        pthread_mutex_lock (&mutexsum);
        dups++;
        pthread_mutex_unlock (&mutexsum);
        pthread_exit(NULL); 
    }
    else
    {
        pthread_exit(NULL);
    }
    
}

//valida columna
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
        printf("Numero duplicado en columna %d\n",col);
        pthread_mutex_lock (&mutexsum);
        dups++;
        pthread_mutex_unlock (&mutexsum);
        pthread_exit(NULL); 
    }
    else
    {
        pthread_exit(NULL);
    }
}

void *revisarMatriz(void* param){
    parameters *params = (parameters*) param;
	int i= params->row;
	int j= params->column;
    int matrix[9]={0,0,0,0,0,0,0,0,0};
    int index=0;
    //recorro la matriz 3x3 con el offset de i y j del for en main
    for (int k = 0; k < 3; k++)
    {
        for (int l = 0; l < 3; l++)
        {
            matrix[index]=sudoku_resuelto[k+i][l+j];
            index++;
        }
    }
    //reviso duplicados en la lista
    int dup=duplicado(matrix);
    if (dup>0){
        printf("Número duplicado en Matriz con inicio en columna %d fila %d\n",i,j);
        pthread_mutex_lock (&mutexsum);
        dups++;//escribo en dups si hay duplicados
        pthread_mutex_unlock (&mutexsum);
        pthread_exit(NULL); 
    }
    else
    {
        pthread_exit(NULL);
    }
    printf("\n----------\n");
}
//retorno el índice del duplicado
int duplicado(int* lista)
{
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

