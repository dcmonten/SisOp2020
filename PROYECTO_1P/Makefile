schedsim:
	gcc -std=c11 -o sched_basics.o -c sched_basics.c
	gcc -std=c11 -I ./libs schedsim.c sched_basics.o -o schedsim 

schedgen:  
	gcc -std=c11 -o schedgen schedgen.c

schedstats:
	gcc -std=c11 -o sched_basics.o -c sched_basics.c
	gcc -std=c11 -I ./libs schedstats.c sched_basics.o -o schedstats 

all: 
	gcc -std=c11 -o sched_basics.o -c sched_basics.c
	gcc -std=c11 -I ./libs schedsim.c sched_basics.o -o schedsim 
	gcc -std=c11 -I ./libs schedstats.c sched_basics.o -o schedstats
	gcc -std=c11 -o schedgen schedgen.c 
	
clean:
	rm -f schedsim
	rm -f schedgen
	rm -f schedstats
	rm -f *.o


