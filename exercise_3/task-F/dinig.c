//usr/bin/clang "$0" -o double_checked -std=gnu11 -g -O3 -lpthread && exec ./double_checked
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define NUM_FORKS 5

static inline void nonOptimizedBusyWait(void){
    for(long i = 0; i < 10000000; i++){
        // "Memory clobber" - tells the compiler optimizer that all the memory 
        // is being touched, and that therefore the loop cannot be optimized out
        asm volatile("" ::: "memory");
    }
}

static pthread_mutex_t forks[NUM_FORKS];

void phil(int i){
    printf("Phil %d is waiting for fork %d\n",i,i);
    pthread_mutex_lock(&forks[i]);
    printf("Phil %d took fork %i\n",i,i);
    int next_fork = i+1;
    if(i>=NUM_FORKS-1){
        printf("table looped for forks\n");
        next_fork = 0;
    }
    nonOptimizedBusyWait();
    printf("Phil %d is waiting for fork %d\n",i,next_fork);
    pthread_mutex_lock(&forks[next_fork]);
    printf("Phil %d took fork %i\n",i,next_fork);
    nonOptimizedBusyWait();
    printf("Phil %d released fork %i\n",i,next_fork);
    pthread_mutex_unlock(&forks[i]);
    printf("Phil %d released fork %i\n",i,next_fork);
    pthread_mutex_unlock(&forks[next_fork]);
}

int main(){
    long forksNum = sizeof(forks)/sizeof(*forks);

    for(long i = 0; i < forksNum; i++){
        pthread_mutex_init(&forks[i], NULL);
    }


    pthread_t threadHandles[5];
    long numThreads = sizeof(threadHandles)/sizeof(*threadHandles);

    for(long i = 0; i < numThreads; i++){
        pthread_create(&threadHandles[i], NULL, phil, i);
    }

    for(long i = 0; i < numThreads; i++){
        pthread_join(threadHandles[i], NULL);
    }    
}

