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

sem_t              empty;              ///<High when bathroom empty*/
sem_t              maleMultiplex;      ///<Male counter*/
sem_t              femaleMultiplex;    ///<Female counter*/
sem_t              noMale;             ///<Locked by female to block male entry
sem_t              noFemale;           ///<Locked by male to block female entry
sem_t              gate;               ///<Gate blocking threads leaving*/


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


void is_man();
void is_woman();





#define MAXTHREADS 20;

struct lightswitch maleSwitch;         ///<Males only lock*/
struct lightswitch femaleSwitch;       ///<Females only lock*/
struct turnstile   gatelock;
    
/******************************************************************************
 *************************************** Main *********************************
 ******************************************************************************/

int main(int argc, char const *argv[]){
    sem_init(&empty, 0, 1);
    sem_init(&noMale,0, 1);
    sem_init(&noFemale, 0, 1);
    sem_init(&maleMultiplex, 0, 3);
    sem_init(&femaleMultiplex, 0, 3);
    sem_init(&maleSwitch.mutex, 0, 1);
    sem_init(&femaleSwitch.mutex, 0, 1);

    sem_init(&gatelock.mutex, 0, 1);
    sem_init(&gatelock.turn1, 0, 0);
    sem_init(&gatelock.turn2, 0, 0);
    gatelock.max = 3;
    gatelock.counter = 0;


    int i = 0;
    int max =  MAXTHREADS
    int males = 0;
    int females = 0;
    pthread_t *threads;
    threads = malloc( sizeof(pthread_t) * max );

    // spawn males
    for (i = 0; i < max; i++){
        if(1){
            males++;
            printf("Creating MALE: %d\n", males);
            pthread_create(&threads[i], NULL, (void*)is_man, NULL);
        }
        else {
            females++;
            printf("Creating FEMALE: %d\n", females);
            pthread_create(&threads[i], NULL, (void*)is_woman, NULL);
        }
    }

    // catch any threads that close
    for (i = 0; i < max; i++){
        pthread_join(threads[i], NULL);
    }

    return 0;
}


/**
 * @brief      Male thread execution
 */
void is_man(){
    while(1){
   // sem_wait(&empty);                   // get in line
    
    sem_wait(&noMale);                  // make sure female haven't locked the bathroom
    ls_lock(&maleSwitch, &noFemale);    // block female entry
    sem_post(&noMale);                  // let another guy check if bathroom is available
    
    sem_post(&empty);                   // get out of line

    sem_wait(&maleMultiplex);           // Wait if too many men in bathroom
    
    // take care of business
    printf("Going in bathroom\n");
    sleep(rand() % 5 + 2);
    printf("Washing hands\n");
    turnstile_wait(&gatelock);
    printf("Out of bathroom\n");


    sem_post(&maleMultiplex);           // done using bathroom
    ls_unlock(&maleSwitch, &noFemale);   // exit bathroom
    
    sleep(rand() % 20 + 5); // don't have to use bathroom for a while
    }
}


/**
 * @brief      Female thread execution
 */
void is_woman(){
    while(1){
//    sem_wait(&empty);                   // get in line
    
    sem_wait(&noFemale);                // make sure male haven't locked the bathroom
    ls_lock(&femaleSwitch, &noMale);    // block male entry
    sem_post(&noFemale);                // let another guy check if bathroom is available
    
    sem_post(&empty);                   // get out of line

    sem_wait(&femaleMultiplex);         // Wait if too many women in bathroom
    
    // take care of business
    printf("Female in\n");
    sleep(rand() % 5 + 2);
    printf("Female out\n");

    sem_post(&femaleMultiplex);         // done using bathroom
    ls_unlock(&femaleSwitch, &noMale);   // exit bathroom
    
    sleep(rand() % 20 + 5);
    }
}
