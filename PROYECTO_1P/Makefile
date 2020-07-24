schedsim:
	gcc -I ./libs schedsim.c -o schedsim 

schedgen:  
	gcc -o schedgen schedgen.c

schedlib:
	gcc -shared -o libsched.so -I ./libs -fPIC sched.c
clean:
	rm -f schedsim
	rm -f schedgen


