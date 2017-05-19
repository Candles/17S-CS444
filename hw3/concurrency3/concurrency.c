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


typedef struct 
{
    int counter;
    sem_t mutex;

}LightSwitch;

/**
 * @brief      Multi-thread semaphore controller - LOCK
 *
 * @param      ls    Pointer to LightSwitch struct
 * @param      sem   The semaphore to be locked
 */
void ls_lock(LightSwitch *ls, sem_t *sem){
    sem_wait(&ls->mutex);
    ls->counter++;
    if (ls->counter == 1)
    {
        sem_wait(sem);
    }
    sem_post(&ls->mutex);
}


/**
 * @brief      Multi-thread semaphore controller - UNLOCK
 *
 * @param      ls    Pointer to LightSwitch struct
 * @param      sem   The semaphore to be unlocked
 */
void ls_unlock(LightSwitch *ls, sem_t* sem){
    sem_wait(&ls->mutex);
    ls->counter--;
    if (ls->counter == 0)
    {
         sem_post(sem);
    }
    sem_post(&ls->mut);
}


sem_t insertMutex;
sem_t noSearcher;
sem_t noInserter;

struct LightSwitch searchSwitch;
struct LightSwitch insertSwitch;

int main( int argc, char *argv[]){
	int i = 0;
	char name[9];
	int worker[MAXTHREADS];
	pthread_t worker_thread[MAXTHREADS];
	
	sem_init(&insertMutex, 0, 1);
	sem_init(&noSearcher, 0, 1);
	sem_init(&noInserter, 0, 1);


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

/**
 * @brief      Directs threads to their designated functions.
 *
 * @param      arg   Worker objective
 */
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

/**
 * @brief      Locks the noSearcher sem with the ability to add more searchers.
 * 
 */
void search(){
	ls_lock(searchSwitch, noSearcher);
	printf("Searching for something!\n");
	printf("Done searching\n");
	ls_unlock(searchSwitch, noSearcher);

}

/**
 * @brief      Locks noInserter sem then blocks until it can grab inserMutex.
 */
void insert(){
	ls_lock(insertSwitch, noInserter);
	sem_wait(insertMutex); 				//wait for other inserts to finish
	printf("Inserting!\n");
	sem_post(insertMutex);
	ls_unlock(insertSwitch, noInserter);
}

/**
 * @brief      Waits for noSearcher and noInsert before deleting.
 */
void delete(){
	sem_wait(noSearcher);
	sem_wait(noInserter);
	printf("Deleting!\n");
	sem_post(noInserter);
	sem_post(noSearcher);
}
