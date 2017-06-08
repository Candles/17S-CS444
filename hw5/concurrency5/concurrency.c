#include "stdio.h"
#include "pthread.h"
#include "unistd.h"
#include "time.h"
#include "stdlib.h"
#include "semaphore.h"


/**
 * Light switch
 */
typedef struct lightswitch {
    int counter;
    sem_t mutex;
} lightswitch;


typedef struct turnstile {
    int counter;
    int max;
    sem_t mutex;
    sem_t turn1;
    sem_t turn2;
} turnstile;


sem_t   agentSem;         ///<Active agent lock*/
sem_t   tabaco;
sem_t   paper;
sem_t   match;
sem_t   tabacoPush;
sem_t   paperPush;
sem_t   matchPush;
int     isTabaco;
int     isPaper;
int     isMatch;
sem_t   invLock;


void pushTabaco();
void pushPaper();
void pushMatch();
void needTabaco();
void needPaper();
void needMatch();
void lightup();
void makecig();
void placeTabacoAndPaper();
void placeMatchAndPaper();
void placeTabacoAndMatch();


/**
 * @brief      Multi-thread semaphore controller - LOCK
 *
 * @param      ls    Pointer to LightSwitch struct
 * @param      sem   The semaphore to be locked
 */
void ls_lock(lightswitch *ls, sem_t *sem){
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
 * @param      ls    Pointer to lightswitch struct
 * @param      sem   The semaphore to be unlocked
 */
void ls_unlock(lightswitch *ls, sem_t* sem){
    sem_wait(&ls->mutex);
    ls->counter--;
    if (ls->counter == 0) {
        sem_post(sem);
    }
    sem_post(&ls->mutex);
}

void turnstile_phase1(turnstile *ls){
    sem_wait(&ls->mutex);
    ls->counter++;
    printf("%d\n", ls->counter);
    if (ls->counter == 3){
        // printf("posting turn1\n");
        sem_post(&ls->turn1);  //lock the gate
        sem_post(&ls->turn1);  //lock the gate
        sem_post(&ls->turn1);  //lock the gate

    }
        sem_post(&ls->mutex);
        sem_wait(&ls->turn1);
        // printf("leaving phase 1\n");
}

void turnstile_phase2(turnstile *ls){
    sem_wait(&ls->mutex);
    ls->counter--;
    // printf("in phase2 %d\n", ls->counter);
    if(ls->counter == 0){
        sem_post(&ls->turn2);
        sem_post(&ls->turn2);
        sem_post(&ls->turn2);
    }
    sem_post(&ls->mutex);
    sem_wait(&ls->turn2);
}

void turnstile_wait(turnstile *ls){
    turnstile_phase1(ls);
    turnstile_phase2(ls);
}


#define MAXTHREADS 9;

    
/******************************************************************************
 *************************************** Main *********************************
 ******************************************************************************/

int main(int argc, char const *argv[]){
    sem_init(&agentSem, 0, 1);
    sem_init(&invLock, 0, 1);
    
    sem_init(&tabacoPush, 0, 0);
    sem_init(&paperPush, 0, 0);
    sem_init(&matchPush, 0, 0);

    sem_init(&tabaco, 0, 0);
    sem_init(&paper, 0, 0);
    sem_init(&match, 0, 0);

    isTabaco = isPaper = isMatch = 0;

    int i = 0;
    int max =  MAXTHREADS
    int males = 0;
    int females = 0;
    pthread_t *threads;
    threads = malloc( sizeof(pthread_t) * max );

    pthread_create(&threads[0], NULL, (void*)pushTabaco, NULL);
    pthread_create(&threads[1], NULL, (void*)pushMatch, NULL);
    pthread_create(&threads[2], NULL, (void*)pushPaper, NULL);

    pthread_create(&threads[6], NULL, (void*)placeTabacoAndPaper, NULL);
    pthread_create(&threads[7], NULL, (void*)placeTabacoAndMatch, NULL);
    pthread_create(&threads[8], NULL, (void*)placeMatchAndPaper, NULL);

    pthread_create(&threads[3], NULL, (void*)needTabaco, NULL);
    pthread_create(&threads[4], NULL, (void*)needMatch, NULL);
    pthread_create(&threads[5], NULL, (void*)needPaper, NULL);

    // catch any threads that close
    for (i = 0; i < max; i++){
        pthread_join(threads[i], NULL);
    }

    return 0;
}


void pushTabaco(){
    while(1){
        sem_wait(&tabacoPush);
        sem_wait(&invLock);
            if(isPaper){
                isPaper = 0;
                sem_post(&match);
            }
            else if (isMatch){
                isMatch = 0;
                sem_post(&paper);
            }
            else{
                isTabaco = 1;
            }
        sem_post(&invLock);
    }
}

void pushPaper(){
    while(1){    
        sem_wait(&paperPush);
        sem_wait(&invLock);
            if(isTabaco){
                isTabaco = 0;
                sem_post(&match);
            }
            else if (isMatch){
                isMatch = 0;
                sem_post(&tabaco);
            }
            else{
                isPaper = 1;
            }
        sem_post(&invLock);
    }
}

void pushMatch(){
    while(1){
        sem_wait(&matchPush);
        sem_wait(&invLock);
            if(isPaper){
                isPaper = 0;
                sem_post(&tabaco);
            }
            else if (isTabaco){
                isTabaco = 0;
                sem_post(&paper);
            }
            else{
                isMatch = 1;
            }
        sem_post(&invLock);
    }
}


void needTabaco(){
    while(1){    
        sem_wait(&tabaco);
        printf("Grabbing MATCH and PAPER\n");
        makecig();
        sem_post(&agentSem);
        lightup();
    }
}

void needPaper(){
    while(1){    
        sem_wait(&paper);
        printf("Grabbing TABACCO and MATCH\n");
        makecig();
        sem_post(&agentSem);
        lightup();
    }
}

void needMatch(){
    while(1){    
        sem_wait(&match);
        printf("Grabbing TABACCO and PAPER\n");
        makecig();
        sem_post(&agentSem);
        lightup();
    }
}


void placeTabacoAndPaper(){
    while(1){
        sem_wait(&agentSem);
        printf("Placing TABACCO and PAPER\n");
        sem_post(&tabacoPush);
        sem_post(&paperPush);
    }
}

void placeMatchAndPaper(){
    while(1){
        sem_wait(&agentSem);
        printf("Placing MATCH and PAPER\n");
        sem_post(&matchPush);
        sem_post(&paperPush);
    }
}

void placeTabacoAndMatch(){
    while(1){
        sem_wait(&agentSem);
        printf("Placing TABACCO and MATCH\n"); 
        sem_post(&tabacoPush);
        sem_post(&matchPush);
    }
}


void makecig(){
    printf("Making cig...\n");
    sleep(rand() % 5 + 1);
}

void lightup(){
    printf("Lighting up...\n");
    sleep(rand() % 7 + 1);
}