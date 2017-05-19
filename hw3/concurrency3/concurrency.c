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

sem_t list_lock_modify; //lock if ANY changes are being made
sem_t list_lock_delete;	//lock if you are a deleter and currently deleting


int main( int argc, char *argv[]){
	int i = 0;
	char name[9];
	int worker[MAXTHREADS];
	pthread_t worker_thread[MAXTHREADS];
	sem_init(&list_lock_modify, 0, 0);
	sem_init(&list_lock_delete, 0, 0);

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
		
		worker[i] = rand() % 2;	// assign workers their role; not promises about there being at least one of each...
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
			case SEARCHER:
				search();
				break;
			case INSTER:
				insert();
				break;
			case DELETER:
				delete();
				break;		
		}//switch
	}//while
}//work

//needs a way to tell if delete is running or not
void search(){
	sval_t temp = 0;

	if (sem_trywait(list_lock_delete) == 0){
		printf("looking for a thing");
	} else {		//deleter active
	       		 	//don't search
	}

}

void insert(){
	if (sem_trywait(list_lock_modify) == 0){
		struct list_head *new = malloc(sizeof(struct list_head));
		list_add_tail(new,listhead);
		sem_post(list_lock_modify);
	}
}

void delete(){
	if (sem_trywait(list_lock_modify)){		// insert isn't inserting or another deleter isn't deleting
		sem_wait(list_lock_delete);			// no search running

	}
}
