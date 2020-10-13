#define _GNU_SOURCE

#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <sched.h>
#include <sys/times.h>
#include <time.h>
#include <x86intrin.h>

#include "io.h"


#define CH_A 1
#define CH_B 2
#define CH_C 3
#define DELAY_TIME 5000

struct timespec timespec_normalized(time_t sec, long nsec){
    while(nsec >= 1000000000){
        nsec -= 1000000000;
        ++sec;
    }
    while(nsec < 0){
        nsec += 1000000000;
        --sec;
    }
    return (struct timespec){sec, nsec};
}


struct timespec timespec_sub(struct timespec lhs, struct timespec rhs){
    return timespec_normalized(lhs.tv_sec - rhs.tv_sec, lhs.tv_nsec - rhs.tv_nsec);
}


struct timespec timespec_add(struct timespec lhs, struct timespec rhs){
    return timespec_normalized(lhs.tv_sec + rhs.tv_sec, lhs.tv_nsec + rhs.tv_nsec);
}


int timespec_cmp(struct timespec lhs, struct timespec rhs){
    if (lhs.tv_sec < rhs.tv_sec)
        return -1;
    if (lhs.tv_sec > rhs.tv_sec)
        return 1;
    return lhs.tv_nsec - rhs.tv_nsec;
}

int set_cpu(int cpu_number)
{
    cpu_set_t cpu;
    CPU_ZERO(&cpu);
    CPU_SET(cpu_number, &cpu);
    return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

static inline void nonOptimizedBusyWait(void){
    for(long i = 0; i < 10000000; i++){
        // "Memory clobber" - tells the compiler optimizer that all the memory 
        // is being touched, and that therefore the loop cannot be optimized out
        asm volatile("" ::: "memory");
    }
}

void* disturbance(){
    while(1){
        asm volatile("" ::: "memory");
    }
}

// Tasks with sleeps
////////////////////
void* task_A()
{
    printf("Start A\n");
    while(1){

        while(io_read(CH_A)){
            //Busy wait
        }

        io_write(CH_A,0);
        usleep(DELAY_TIME); 
        io_write(CH_A,1);
    }
}

void* task_B()
{
    printf("Start B\n");
    while(1){
        while(io_read(CH_B)){
            //Busy wait
        }
        io_write(CH_B,0);
        usleep(DELAY_TIME); 
        io_write(CH_B,1);
    }
}

void* task_C()
{
    while(1){

        while(io_read(CH_C)){
            //Busy wait
        }
        io_write(CH_C,0);
        usleep(DELAY_TIME); 
        io_write(CH_C,1);
    }
}


// Tasks with periodic sleeps
////////////////////
void* task_A_periodic()
{
    struct timespec waketime = {0};
    clock_gettime(CLOCK_REALTIME, &waketime);

    struct timespec period = {
        .tv_sec = 0,
        .tv_nsec = 1 * 1000 * 1000
    };

    printf("Start A\n");
    while(1){

        if(!io_read(CH_A)){
            //Busy wait

            io_write(CH_A,0);
            usleep(DELAY_TIME); 
            io_write(CH_A,1);
        }

        waketime = timespec_add(waketime, period);
        clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &waketime, NULL);
    }
}

void* task_B_periodic()
{
    struct timespec waketime = {0};
    clock_gettime(CLOCK_REALTIME, &waketime);

    struct timespec period = {
        .tv_sec = 0,
        .tv_nsec = 1 * 1000 * 1000
    };

    printf("Start B\n");
    while(1){
        if(!io_read(CH_B)){
            //Busy wait
            io_write(CH_B,0);
            usleep(DELAY_TIME); 
            io_write(CH_B,1);
        }

        waketime = timespec_add(waketime, period);
        clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &waketime, NULL);
    }
}

void* task_C_periodic()
{
    struct timespec waketime = {0};
    clock_gettime(CLOCK_REALTIME, &waketime);

    struct timespec period = {
        .tv_sec = 0,
        .tv_nsec = 1 * 1000 * 1000
    };
    while(1){

        if(!io_read(CH_C)){
            //Busy wait
            io_write(CH_C,0);
            usleep(DELAY_TIME); 
            io_write(CH_C,1);

        }
        waketime = timespec_add(waketime, period);
        clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &waketime, NULL);
    }
}

void* the_500_ms_func()
{
    struct timespec waketime = {0};
    clock_gettime(CLOCK_REALTIME, &waketime);

    struct timespec period = {
        .tv_sec = 0,
        .tv_nsec = 500 * 1000 * 1000
    };

    while(1) {
        printf("500ms has passed!\n");

        waketime = timespec_add(waketime, period);
        clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &waketime, NULL);
    }
}

#define DISTURBANCE 1
#define PERIODIC 1

int main()
{
    io_init();
    set_cpu(1);

    pthread_t disturbances[10]; 
    pthread_t threadHandleA, threadHandleB, threadHandleC;
    pthread_t threadHandleA_periodic, threadHandleB_periodic, threadHandleC_periodic;
    if(!PERIODIC)
    {
        pthread_create(&threadHandleA, NULL, task_A, NULL);
        pthread_create(&threadHandleB, NULL, task_B, NULL);
        pthread_create(&threadHandleC, NULL, task_C, NULL);
    } else
    {

        pthread_create(&threadHandleA_periodic, NULL, task_A_periodic, NULL);
        pthread_create(&threadHandleB_periodic, NULL, task_B_periodic, NULL);
        pthread_create(&threadHandleC_periodic, NULL, task_C_periodic, NULL);
    }

    if(DISTURBANCE)
    {
        for(int i=0; i<10; i++)
        {
            pthread_create(&disturbances[i], NULL ,disturbance, NULL);
        }
    }
    if(!PERIODIC)
    {
        pthread_join(threadHandleA, NULL);
        pthread_join(threadHandleB, NULL);
        pthread_join(threadHandleC, NULL);
    } else {

        pthread_join(threadHandleA_periodic, NULL);
        pthread_join(threadHandleB_periodic, NULL);
        pthread_join(threadHandleC_periodic, NULL);
    }

    if(DISTURBANCE)
    {
        for(int i=0; i<10; i++)
        {
            pthread_join(disturbances[i], NULL);
        }
    }
}
