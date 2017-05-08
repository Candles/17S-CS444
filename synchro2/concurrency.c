//======================================================//
//Spring 2017 CS-444                                    //
//Assignment #2                                         //
//                                                      //
//                                                      //
//By Chongxian Chen, Thomas Olson and Christopher Tang	//
//======================================================//

#include "stdio.h"
#include "pthread.h"
#include "unistd.h"
#include "time.h"
#include "stdlib.h"
#include "semaphore.h"

void think();
void get_forks();
void eat();
void put_forks();
void *philosopher(void*);

pthread_mutex_t forks[5];

sem_t phil_eat[5];
sem_t phil_finished[5];

const char *names[] = {"Wittgenstein", "Rawls", "Russell", "Kant", "Diogenes"};

int main( int argc, char *argv[]){


    pthread_t phil_ids[5];


    for(int i = 0; i < 5; i++){
	sem_init(&phil_eat[i], 0, 0);
	sem_init(&phil_finished[i], 0, 0);
	pthread_mutex_init(&forks[i], NULL);
	
	int *phil = malloc(sizeof(int));
	*phil = i;

	pthread_create(&phil_ids[i], NULL, philosopher, (void*)phil);
    }


    int phil = 0;

    //The main thread will act as a "waiter", giving permission to the philsophers to eat
    //in pairs. This method simply repats the order {(0, 2), (4, 1), (3, 0), (2, 4), (1, 3)}
    //for which philosophers get to eat, guaranteeing each philosopher eats the same number
    //of times without deadlocking.


    while(1){ 

	sem_post(&phil_eat[phil]);
	sem_post(&phil_eat[(phil+2)%5]);

	sem_wait(&phil_finished[phil]);      		 
	sem_wait(&phil_finished[(phil+2)%5]);

	sleep(1);

	phil = (phil + 4) % 5;
    }

    return 0;
}

void think(int phil){
    printf("%s is thinking\n", names[phil]);
    sleep(rand() % 20 + 1);
    printf("%s is done thinking\n", names[phil]);
}

void get_forks(int phil){
    sem_wait(&phil_eat[phil]);
    pthread_mutex_lock(&forks[phil]);
    printf("Fork %d is in use\n", phil);
    pthread_mutex_lock(&forks[(phil+1)%5]);
    printf("Fork %d is in use\n", (phil+1)%5);
}

void eat(int phil){
    printf("%s is eating\n", names[phil]);
    sleep(rand() % 8 + 2);
    printf("%s is done eating\n", names[phil]);
}

void put_forks(int phil){
    pthread_mutex_unlock(&forks[phil]);
    printf("Fork %d is no longer in use\n", phil);
    pthread_mutex_unlock(&forks[(phil+1)%5]);    
    printf("Fork %d is no longer in use\n", (phil+1)%5);
    sem_post(&phil_finished[phil]);
}

void *philosopher(void* arg){
    int phil = *((int*)arg);
    while(1){
	think(phil);
	get_forks(phil);
	eat(phil);
	put_forks(phil);
    }
}
