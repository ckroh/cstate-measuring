#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <math.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <omp.h>

#define NUM_MEASUREMENTS 64

void execute(char **argv)
{
     pid_t  pid;
     int    status;

     if ((pid = fork()) < 0) {    
          printf("*** ERROR: forking child process failed\n");
          exit(1);
     }
     else if (pid == 0) {         
          if (execvp(*argv, argv) < 0) {     
               printf("*** ERROR: exec failed\n");
               exit(1);
          }
     }
     else {                                 
          while (wait(&status) != pid);
     }
}


void writeCache(uint64_t val, uint64_t *dc, uint64_t cache_size){
	
	
	for(int i = 0; i < cache_size; i++){
		dc[i] = (uint64_t) (val + i);
	}

}

void work(uint64_t val){
	unsigned long long int res = 1;
	for(int i = 0; i<val*128; i++){
		res += res * pow(val,i);
	}
}

void workHard(uint64_t val){
	unsigned long long int res = 1;
	for(int i = 0; i<val*128; i++){
		res = pow(val,i);
	}
}

int main(int argc, char *argv[] ){
	if(argc<3){
		printf("usage: %s rand_num cstate cache_size\n", argv[0]);
		return 1;
	}
	char *cstateson[] = { "sudo", "sh", "/home/s1428123/Beleg/Programm/limit_cstate.sh", "-on", argv[2], NULL };
	
	char *cstatesoff[] = { "sudo", "sh", "/home/s1428123/Beleg/Programm/limit_cstate.sh", "-off", NULL };
	uint64_t val;
	uint64_t *dc1, *dc2;
	uint64_t cache_size = 0;
	
	
	val = strtoul(argv[1],NULL,10);
	
	if(argc>2){
		cache_size = strtoul(argv[3],NULL,10);
	}
	
	execute(cstateson);
	
	printf("writing cache with %lu bytes\n", (8*cache_size));

	#pragma omp parallel 
	{
		switch(omp_get_thread_num()){
			//core 0: write Cache, work and sleep -> core c-states
			case 0:
				dc1 = malloc(sizeof(uint64_t) * cache_size);
				for(int i = 0; i< NUM_MEASUREMENTS; i++){
					writeCache(val, dc1, cache_size);
					usleep(600000);
					work(val);
					usleep(20);
					workHard(val);
				}
				free(dc1);
				break;
	
			//core 1: busy half the time, idle other half -> package c-states
			case 1:
				work(val*3*NUM_MEASUREMENTS);
				usleep(400000*NUM_MEASUREMENTS-1000);
				break;
		
			//thread for 2nd socket: ensures work on 2nd socket
			case 2:
				work(val*4*NUM_MEASUREMENTS);
				//usleep(400000*NUM_MEASUREMENTS-5000);
				break;
		
			default:
				usleep(60000000);
				break;
		
		}
	}
		
		
	execute(cstatesoff);
	
 	return 0;
}
 