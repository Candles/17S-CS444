//======================================================//
//Spring 2017 CS-444                                    //
//Assignment #3                                         //
//                                                      //
//                                                      //
//By Chongxian Chen, Thomas Olson and Christopher Tang	//
//======================================================//


/**
 * Searchers, Inserters and Deleters.
 * 
 * Searchers merely examine the list; hence they can execute concurrently with each other.
 * 
 * Inserters add new items to the end of the list; insertions must be mutually exclusive to preclude
 * two inserters from inserting new items at about the same time.
 * However, one insert can proceed in parallel with any number of searches. 
 * 
 * Deleters remove items from anywhere in the list. At most one deleter process can access the list
 * at a time, and deletion must also be mutually exclusive with searches and insertions.
 * 
 * simplified:
 * insert can coexist with search
 * delete must be by itself
 * insert cant be with another insert
 * 
 * 	|||\\\||| insert | delete | search
 * 	----------------------------------
 * 	insert ||		 |		  |   x 
 * 	----------------------------------
 * 	delete ||        |   x	  |
 * 	----------------------------------
 * 	search ||   x    |		  |   x
 * 	----------------------------------
 */


#include "stdio.h"
#include "pthread.h"
#include "unistd.h"
#include "time.h"
#include "stdlib.h"
#include "list.h"
#include "semaphore.h"

#define MAXTHREADS  8

#define SEARCHER 	0
#define INSERTER 	1
#define DELETER  	2

#define LIST_HEAD(listhead)
struct list_head listhead = LIST_HEAD_INIT(listhead)

sem_t list_lock;


int main( int argc, char *argv[]){
	int i = 0;
	char name[9];
	int worker[MAXTHREADS];
	pthread_t worker_thread[MAXTHREADS];
	sem_init(&list_lock, 0, 0);

	INIT_LIST_HEAD(*listhead);

	for(;i < MAXTHREADS; i++){
		switch worker[i] {
			case 0:
				sprintf(name, "Searcher");
				break;
			case 0:
				sprintf(name, "Inserter");
				break;
			case 0:
				sprintf(name, "Deleter");
				break;
		}
		printf("Creating %s\n", name);
		
		worker[i] = rand() % 2;	// assign workers their role
		pthread_create(&worker_thread[i], NULL, work, (void*)worker[i]); // create workers and start them
 	}

    while(1){ 

    }

    return 0;
}




void *wrok(void* arg){
    int objective = *((int*)arg);
    while(1){
		switch objective{
			case 0:
				break;
			case 1:
				break;
			case 2:
				break;		
		}//switch
	}//while
}//work


void search(){
	sval_t temp = 0;

	sem_getvalue(list_lock, temp);
	if (val == SEARCHER || val == INSERTER){
		//DO SHEARCHING
		printf("looing for a %c", letter)
	} else {//deleter
	       //dont search
	}

}

void insert(){
	if (sem_trylock(list_lock) == 0){

	}
}

