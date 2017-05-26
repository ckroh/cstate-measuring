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
#include <xmmintrin.h>

/*defines the number of transitions between work and sleep of thread 0*/
#define NUM_MEASUREMENTS 64

/*method to start another process */
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

/*writes amount of data to simulate cache load*/
/*@param val 		random value */
/*@param dc 		pointer to array*/
/*@param cache_size 	amount of data to be written*/
void writeCache(uint64_t val, uint64_t *dc, uint64_t cache_size){
	for(int i = 0; i < cache_size; i++){
		dc[i] = (uint64_t) (val + i);
	}
}

/*creates constant workload using arithmetric instructions*/
/*@param val 	random integer to prevent compiler optimizations */
void work(uint64_t val){
	unsigned long long int res = 1;
	for(int i = 0; i<val*64; i++){
		res += res * pow(val,i);
	}
}

/*creates different workload than work(val) by using AVX instructions*/
/*@param val 	random integer to prevent compiler optimizations */
void workHard(uint64_t val){
	__m128 vector1 = _mm_setr_ps(val+4, val+3, val+2, val+1);
	__m128 vector2 = _mm_setr_ps(val+7, val+8, val+9, 0);
	for(int i = 0; i<val*128; i++){
		vector1 = _mm_mul_ps(vector1, vector2); 
	}
}

/*main mehtod expecting 3 arguments*/
/*@arg1 rand_num		random integer that is used in workload functions to prevent compiler optimizations */
/*@arg2 cstate		C-state that shall be observed, disables others using shell script. Accepted values are c1,c1e,c3,c6 */
/*@arg3 cache_size	amount of data that shall be written to simulate dirty cache in  */
int main(int argc, char *argv[] ){
	if(argc<3){
		printf("usage: %s rand_num cstate cache_size\n", argv[0]);
		return 1;
	}
	char *cstateson[] = { "sudo", "sh", "/home/s1428123/Beleg/Programm/cstate-measuring/limit_cstate.sh", "-on", argv[2], NULL };
	
	char *cstatesoff[] = { "sudo", "sh", "/home/s1428123/Beleg/Programm/cstate-measuring/limit_cstate.sh", "-off", NULL };
	uint64_t val;
	uint64_t *dc1, *dc2;
	uint64_t cache_size = 0;
	
	
	val = strtoul(argv[1],NULL,10);
	
	if(argc>2){
		cache_size = strtoul(argv[3],NULL,10);
	}
	
/*	disable unwanted C-states*/
	execute(cstateson);
		
	printf("writing cache with %lu bytes\n", (8*cache_size));
	
/*	start parallel section with 3 threads*/
	#pragma omp parallel private(dc1)
	{
		switch(omp_get_thread_num()){
/*			core 0: write cache, work and sleep -> core c-states*/
			case 0:
				dc1 = malloc(sizeof(uint64_t) * cache_size);
				if(dc1==NULL){
					printf("Error: malloc failed\n"); break;
				}
				for(int i = 0; i< NUM_MEASUREMENTS; i++){
					writeCache(val, dc1, cache_size);
/*					usleep suggests to OS that thread will be sleeping, triggering processor idle state*/
					usleep(600000);
/*					wake-up again and generate even workload*/
					work(val);
/*					create signature workload in Score-P trace to synchronize timestamps*/
					workHard(val);
					work(val);
				}
				free(dc1);
				break;
	
/*			core 1: busy half the time, idle other half -> package c-states*/
			case 1:
/*				ensure even workload on second core during core C-state measurements on the first core*/
				work(val*5*NUM_MEASUREMENTS);
/*				sleep and suggest processor idle state, now package idle states are measured on thread 0*/
				usleep(400000*NUM_MEASUREMENTS-1000);
				break;
		
/*			thread for 2nd socket: ensures work on 2nd socket*/
			case 2:
				work(val*7*NUM_MEASUREMENTS);
				break;
		
			default:
				usleep(60000000);
				break;
		
		}
	}
/*	enable all C-states again	*/
	execute(cstatesoff);
	
 	return 0;
}
 
