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

#include "semaphore.h"

#define MAXTHREADS  5

#define SEARCHER 	0
#define INSERTER 	1
#define DELETER  	2

void *work(void*);
void search();
void insert();
void delete();


int list_length;
struct node_t *head;

sem_t insertMutex;
sem_t noSearcher;
sem_t noInserter;




typedef struct node_t {
	int 		 data;
	struct node_t *next;
} node_t;


/**
 * @brief      Adds a new node to the tail.
 *
 * @param      head  The head of the list.
 * @param[in]  val   The value to store.
 */
void add_tail(node_t *head, int val){
	node_t *current = head;
	if (head != NULL){
	while (current->next != NULL){
		current = current->next;
	}
	current->next = malloc(sizeof(node_t));
	current->next->data = val;
	current->next->next = NULL;
	list_length++;
	}
}

/**
 * @brief      Looks for the first node that matches val.
 *
 * @param      head  The head of the list.
 * @param[in]  val   The value to match.
 *
 * @return     Returns a pointer to the matched node.
 */
node_t* search_for_node(node_t *head, int val){
	node_t *current = head;
	while (current->data != val){
		if (current->next != NULL){
			current = current->next;
		}
		else {
			return NULL;	
		}
	}
	return current;
}

/**
 * @brief      Removes a node at "random".
 *
 * @param      head  The head of the list.
 */
void remove_node(node_t *head){
	node_t *current = head;
	node_t *temp = NULL;
	int i = 0;
	int j = rand() % list_length;
	
	if (j == 0){
/*
	    if (head == NULL) {
        	return;
    	}

       	temp = (head)->next;
       	free(head);
       	head = temp;
	list_length--;  
*/  
    return;
	}

	else {
		for (; i < j; i++){
			if (current->next != NULL){
				current = current->next;
			} else {
				return;
			}
		}

		temp = current->next;
		current->next = temp->next;
		free(temp);
		list_length--;
	}

}


/**
 * @brief      Prints out a list.
 *
 * @param      head  The head of the list.
 */
void print_list(node_t *head){
	node_t	*current = head;
	while (current->next != NULL){
		printf("%5d", current->data);
		current = current->next;
	}
	printf("%5d", current->data);
	printf("\n");
}


/**
 * @brief      Frees the list.
 *
 * @param      head  The head of the list.
 */
void free_list(node_t *head){
	node_t *temp = head;
	while(head->next != NULL){
		temp = head;
		head = head->next;
		free(temp);
	}
}



typedef struct LightSwitch {
    int counter;
    sem_t mutex;
} LightSwitch;


/**
 * @brief      Multi-thread semaphore controller - LOCK
 *
 * @param      ls    Pointer to LightSwitch struct
 * @param      sem   The semaphore to be locked
 */
void ls_lock(LightSwitch *ls, sem_t *sem){
    sem_wait(&ls->mutex);
    ls->counter++;
    if (ls->counter == 1) {
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
    if (ls->counter == 0) {
        sem_post(sem);
    }
    sem_post(&ls->mutex);
}


struct LightSwitch searchSwitch;
struct LightSwitch insertSwitch;


int main( int argc, char *argv[]){
	int i = 0;
	char name[9];
	pthread_t worker_thread[MAXTHREADS];
	int *workerid;
	head = malloc(sizeof(node_t));
	
	sem_init(&insertMutex, 0, 1);
	sem_init(&noSearcher,  0, 1);
	sem_init(&noInserter,  0, 1);

	sem_init(&searchSwitch.mutex, 0, 1);
	sem_init(&insertSwitch.mutex, 0, 1);	
	

	for(i = 0; i < MAXTHREADS; i++){
		workerid = malloc(sizeof(int)); 
	        *workerid = i % 3;
		switch (*workerid) {
			case 0:
				sprintf(name, "Searcher");
				break;
			case 1:
				sprintf(name, "Inserter");
				break;
			case 2:
				sprintf(name, "Deleter");
				break;
            default:
                sprintf(name, "%d", workerid);
		}
		printf("Creating %s\n", name);

		pthread_create(&worker_thread[i], NULL, work, (void*)workerid); // create workers and start them
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
void *work(void* arg){
    int objective = *((int*)arg);
    while(1){
		switch (objective) {
			case SEARCHER:
				search();
				break;
			case INSERTER:
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
	int val = rand() % 100;

	ls_lock(&searchSwitch, &noSearcher);
	
    printf("Searching for %d ", val);
	if (search_for_node(head, val) != NULL){
		 printf("Found it!\n");
	}
	else{
		 printf("Not found!\n");
	}
	
    ls_unlock(&searchSwitch, &noSearcher);
	sleep(rand() % 8 + 2);
}


/**
 * @brief      Locks noInserter sem then blocks until it can grab inserMutex.
 */
void insert(){
	int val = rand() % 100;

	ls_lock(&insertSwitch, &noInserter);
	sem_wait(&insertMutex); 				//wait for other inserts to finish
	
    printf("Inserting %d!\n", val);
	add_tail(head, val);
	
    sem_post(&insertMutex);
	ls_unlock(&insertSwitch, &noInserter);
	sleep(rand() % 8 + 2);
}


/**
 * @brief      Waits for noSearcher and noInsert before deleting.
 */
void delete(){
	int temp = 0;

	sem_wait(&noSearcher);
	sem_wait(&noInserter);

	printf("Deleting a node!\n");
	remove_node(head);

	sem_post(&noInserter);
	sem_post(&noSearcher);
	sleep(rand() %8 + 2);
}
