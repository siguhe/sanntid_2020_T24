#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>


// Note the argument and return types: 

static long g_long = 0;
sem_t g_long_sem;


void* func_1(void* args)
{
    static long a = 0;
    for (int i = 0; i < 50000000; i++) {
        a++;
       // sem_wait(&g_long_sem);
        g_long++;
        //sem_post(&g_long_sem);
    }
    printf("a: %d, g_long:%ld\n", a, g_long);
}

void* func_2(void* args)
{
    static long b = 0;
    for (int i = 0; i < 50000000; i++) {
        b++;
        //sem_wait(&g_long_sem);
        g_long++;
        //sem_post(&g_long_sem);
    }
    printf("b: %d, g_long:%ld\n", b, g_long);
}


int main()
{
    sem_init(&g_long_sem,0,1);
    pthread_t threadHandle1, threadHandle2;

    pthread_create(&threadHandle1, NULL, func_1, NULL);
    pthread_create(&threadHandle2, NULL, func_2, NULL);

    pthread_join(threadHandle1, NULL);
    pthread_join(threadHandle2, NULL);



    printf("g_long: %ld\n", g_long);
}
