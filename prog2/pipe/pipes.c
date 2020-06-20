#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>

int main( int args, char *argv[] ) {
	//arreglo para escribir y leer en el pipe
	int parr[2];
	// valor de retorno del pipe
	int pipe_retval;
	// fd para el read
	int rd;
	// struct para gettime
    struct timeval current_time;
	//inicializo el tiempo
	gettimeofday(&current_time,NULL);
	//creo el pipe
    if (pipe(parr) < 0)
        exit(1);//por si falla
	else
	{
		
		//creo el hilo
		int pid = fork();
		if ( pid == 0 )
		{
			//escribo el current time inicial en el pipe
            write(parr[1], &current_time.tv_usec, sizeof(int)); 
			execvp( argv[1], &argv[1] ); //ejecuto el comando
			wait( NULL );//hago que execvp vuelva
			gettimeofday(&current_time,NULL);//tomo el current time de nuevo
        	write(parr[1], &current_time.tv_usec, sizeof(int));//lo escribo en el pipe
		}
		else{
			
			int index=0;
			int saved[2];
			close(parr[1]);//cierro el fd del pipe
			//leo el valor del pipe
			while ((rd = read(parr[0], &pipe_retval, sizeof(int))) > 0)
			{
				//guardo el valor
				saved[index] =pipe_retval;
				index++;
			}
			if (rd != 0)
				exit(2);
			//mido el total
			int total=saved[0]-saved[1];
			//imprimo el valor
			printf("Elapsed %d (in usec)\n",total);
		}
	}
	//finalizo el programa
	return 0;
}
