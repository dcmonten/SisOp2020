#include <stdio.h>
#include<stdlib.h>
#include <sys/time.h>
int main()
{
	//defino variables
	int start,end,time_start,time_end,total;
	//current time, struct necesario para gettimeofday
    struct timeval current_time;
	//inicialización del cronometro
    int time_start = gettimeofday(&current_time,NULL);
    //Si no hay error en la creación del inicial
    if (time_start==0){
		//tomo los microsegundos iniciales 
        start=current_time.tv_usec;
        
        //TODO: Put fork and exec to execute terminal command
        
		
		//ENDTODO
		
		//Mido de nuevo el tiempo
        time_end=gettimeofday(&current_time,NULL);
		//Si no hay error en la creacion del final
        if (time_end==0){
			//tomo los microsegundos finales
            end=current_time.tv_usec;
			//mido el tiempo transcurrido entre end y start
			total = end-start;
			//imprimo resultado
            printf("Total: %d",total);
        }
		//si hay error en la creacion del final
		else{
            printf("Error at end gettimeofday");
            exit(-1);
        }
   
    }
	//si hay error en la creacion del inicial
	else{
            printf("Error at start gettimeofday");
            exit(-1);
    }
    
    return 0;
}