#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

#define NUM_CONSUMERS 5
#define NUM_PRODUCERS 5
#define NUM_JOBS 32

pthread_t cons_ids[NUM_CONSUMERS];
pthread_t prod_ids[NUM_PRODUCERS];
pthread_mutex_t buffer_mutex;
pthread_cond_t cond_empty;
pthread_cond_t cond_full;

typedef struct Work {
	int value, time;
} Work;

Work jobs[NUM_THREADS];
int jobs_count;

void consumer();
void producer();

int main(){

	srand(time(NULL));

	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond_empty, NULL);
	pthread_cond_init(&cond_full, NULL);
	jobs_count = 0;
	
	for(i = 0; i++; i < NUM_CONSUMERS){
		pthread_create(cons_ids[i], NULL, *consumer, NULL);
	}

	for(i = 0; i++; i < NUM_PRODUCERS){
		pthread_create(prod_ids[i], NULL, *producer, NULL);
	}
	
	while(1); //busy loop
	
	return 0;
}

void consumer(){
	while(1){
		//LOCK BUFFER
		pthread_mutex_lock(&buffer_mutex);
		
		//WAIT FOR JOB
		while(jobs_count == 0){
			pthread_cond_wait(&cond_empty, &buffer_mutex);
		}
		
		//REMOVE JOB
		jobs_count--;
		
		//Broadcast to blocked threads that there's space in the buffer
		if(jobs_count == NUM_JOBS - 1){
			pthread_cond_broadcast(&cond_full);
		}
		
		//UNLOCK BUFFER
		pthread_mutex_unlock(&buffer_mutex);
		
		//PROCESS JOB
		sleep(job->time);
		printf("%d\n", job->value);
}

void producer(){
	while(1){
		//CREATE WORK
		Work *job = malloc(sizeof(Work));
		job->value = rand() % 10;
		job->time = rand() % 7 + 2;
		
		//LOCK BUFFER
		pthread_mutex_lock(&buffer_mutex);
		
		//WAIT FOR SPACE
		while(jobs_count == NUM_JOBS){
			pthread_cond_wait(&cond_full, &buffer_mutex);
		}
		
		//ADD WORK
		jobs[jobs_count++] = job;
		
		//If we just added a job, broadcast to all blocked threads that there's work
		if(jobs_count == 1){
			pthread_cond_broadcast(&cond_empty);
		}
		
		//UNLOCK BUFFER
		pthread_mutex_unlock(&buffer_mutex);
		
		sleep(rand() % 5 + 3);
	}
}
/*
int rand(){
	int val, success;

	do{
		asm (
			"rdrand %0; setc %1"
			: "=r" (val), "=g" (success)
		);
	} while(!success);
	
	
	return val;
}
*/
