//======================================================//
//Spring 2017 CS-444									//
//Assignment #1											//
//Concurrency											//
//														//
//By Chongxian Chen, Thomas Olson and Christopher Tang	//
//======================================================//

#include "stdio.h"
#include "pthread.h"
#include "unistd.h"
#include "time.h"
#include "stdlib.h"
#include "getopt.h"
#include "mt19937ar.c"

#define NUM_JOBS 32

typedef struct Work {
	int value, time;
} Work;

void *consumer(void*);
void *producer(void*);

int rdrand_support();
unsigned long rand_long();


//Mutex globals
pthread_mutex_t buffer_mutex;
pthread_cond_t cond_empty;
pthread_cond_t cond_full;

Work jobs[NUM_JOBS];
int jobs_count;


int main( int argc, char *argv[]){

	if (argc != 5){
		goto invalidarg;
	}

	int num_consumers, num_producers;
	char opt;
	
	while((opt = getopt(argc, argv, "c:p:")) != -1){
		switch(opt){
		case 'c':
			num_consumers = atoi(optarg);
			if (num_consumers < 1){
				printf("Warning: No consumer threads specified\n");
			}
			break;
		case 'p':
			num_producers = atoi(optarg);
			if (num_producers < 1){
				printf("Warning: No producer threads specified\n");
			}
			break;
		default:
			goto invalidarg;
		}
	}
	
	if(0){
		invalidarg:
		printf("Usage: %s -c <consumer_threads> -p <producer_threads>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	//Seed the generator if we're not using RDRAND
	if (!rdrand_support()){
		init_genrand(time(NULL));
	}

	pthread_t cons_ids[num_consumers];
	pthread_t prod_ids[num_producers];
	pthread_mutex_init(&buffer_mutex, NULL);
	pthread_cond_init(&cond_empty, NULL);
	pthread_cond_init(&cond_full, NULL);
	jobs_count = 0;

	
	for(int i = 0; i < num_consumers; i++){
		pthread_create(&cons_ids[i], NULL, consumer, NULL);
	}

	for(int i = 0; i < num_producers; i++){
		pthread_create(&prod_ids[i], NULL, producer, NULL);
	}


	while(1){ sleep(10); } //busy loop

	return 0;
}

void *consumer(void *args){
	while(1){


		//LOCK BUFFER
		pthread_mutex_lock(&buffer_mutex);

		//WAIT FOR JOB
		while(jobs_count == 0){
			pthread_cond_wait(&cond_empty, &buffer_mutex);
		}

		//REMOVE JOB
		Work job = jobs[--jobs_count];

		//If we just cleared space in a full buffer, let the blocked producers know
		if(jobs_count == NUM_JOBS - 1){
			pthread_cond_broadcast(&cond_full);
		}

		//UNLOCK BUFFER
		pthread_mutex_unlock(&buffer_mutex);

		//PROCESS JOB
		sleep(job.time);
		printf("Value: %d\n", job.value);

	}
}

void *producer(void *args){
	while(1){

		//CREATE WORK
		Work job;
		job.value = rand_long() % 1024;
		job.time = rand_long() % 7 + 2;

		//LOCK BUFFER
		pthread_mutex_lock(&buffer_mutex);

		//WAIT FOR SPACE
		while(jobs_count == NUM_JOBS){
			pthread_cond_wait(&cond_full, &buffer_mutex);
		}

		//ADD WORK
		jobs[jobs_count++] = job;

		//If we just added a job to an empty buffer, let the blocked consumers know
		if(jobs_count == 1){
			pthread_cond_broadcast(&cond_empty);
		}

		//UNLOCK BUFFER
		pthread_mutex_unlock(&buffer_mutex);

		sleep(rand_long() % 5 + 3);
	}
}

int rdrand_support(){
	int eax, ebx, ecx, edx;
	
	__asm__ volatile(
		"cpuid"
		: "=a" (eax),
		  "=b" (ebx),
		  "=c" (ecx),
		  "=d" (edx)
		: "a" (1), "c" (0)
	);
	
	return (ecx & (1 << 30));
}

unsigned long rand_long(){
	unsigned long val;

	if (rdrand_support()){
	
		int success = 0;
	
		do {
			__asm__ (
				"rdrand %0; setc %1"
				: "=r" (val), "=m" (success)
			);
		} while(!success);
	} else {
		val = genrand_int32();
	}
	
	return val;
}