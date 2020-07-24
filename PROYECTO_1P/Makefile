schedsim:
	gcc -I ./libs schedsim.c -o schedsim 

schedalgos:  
	gcc -o schedalgos schedsim_with_lib.c -L./ -lsched 

schedlib:
	gcc -shared -o libsched.so -I ./libs -fPIC sched.c
clean:
	rm -f schedsim

