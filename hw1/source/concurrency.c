#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <getopt.h>

#define NUM_CONSUMERS 5
#define NUM_PRODUCERS 5
#define NUM_JOBS 32


pthread_mutex_t buffer_mutex;
pthread_cond_t cond_empty;
pthread_cond_t cond_full;

typedef struct Work {
	int value, time;
} Work;

Work jobs[NUM_JOBS];
int jobs_count;
void *consumer(void*);
void *producer(void*);

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
	

	srand(time(NULL));

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

		//Broadcast to blocked threads that there's space in the buffer
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
		job.value = rand() % 10;
		job.time = rand() % 7 + 2;


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
