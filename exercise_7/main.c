#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include <native/task.h>
//#include <native/times.h>
#include <sys/mman.h>
#include <sys/times.h>
#include <time.h>
#include <pthread.h>

#define TIME_PERIODIC 1e6
#define TIME_SLEEP 5000

#define CH_A 1
#define CH_B 2
#define CH_C 3

RT_TASK hello_task;

// function to be executed by task
void helloWorld(void *arg)
{
	rt_task_set_periodic(NULL, TM_NOW, 1e9);
	while(1) {
		printf("Hello World! Inside task\n");
		rt_task_wait_period(NULL);
	}
}
//////*****************////////////
////// BUSY-WAIT TASKS **********
//////*****************////////////
void task_A_busy(void *arg)
{
	printf("Inside task A \n");
	while(1) {
		if (!io_read(CH_A)) {
			io_write(CH_A, 0);
			rt_timer_spin(TIME_SLEEP);
			io_write(CH_A, 1);
		} else {
			rt_task_yield();
		}

	}
}

void task_B_busy(void *arg)
{
	printf("Inside task B \n");
	while(1) {
		if (!io_read(CH_B)) {
			io_write(CH_B, 0);
			rt_timer_spin(TIME_SLEEP);
			io_write(CH_B, 1);
		} else {
			rt_task_yield();

		}
	}
}

void task_C_busy(void *arg)
{
	printf("Inside task C \n");
	while(1) {
		if (!io_read(CH_C)) {
			io_write(CH_C, 0);
			rt_timer_spin(TIME_SLEEP);
			io_write(CH_C, 1);
		} else {
			rt_task_yield();
		}

	}
}


//////*****************////////////
////// PERIODIC TASKS **********
//////*****************////////////
void task_A_periodic(void *arg)
{
	rt_task_set_periodic(NULL, TM_NOW, TIME_PERIODIC);
	printf("Inside task A periodic\n");

	while(1) {
		if (!io_read(CH_A)) {
			io_write(CH_A, 0);
			rt_timer_spin(TIME_SLEEP);
			io_write(CH_A, 1);
		}
		rt_task_wait_period(NULL);
	}
}

void task_B_periodic(void *arg)
{
	rt_task_set_periodic(NULL, TM_NOW, TIME_PERIODIC);
	printf("Inside task B periodic\n");

	while(1) {
		if (!io_read(CH_B)) {
			io_write(CH_B, 0);
			rt_timer_spin(TIME_SLEEP);
			io_write(CH_B, 1);
		}
		rt_task_wait_period(NULL);
	}
}

void task_C_periodic(void *arg)
{
	rt_task_set_periodic(NULL, TM_NOW, TIME_PERIODIC);
	printf("Inside task C periodic\n");

	while(1) {
		if (!io_read(CH_C)) {
			io_write(CH_C, 0);
			rt_timer_spin(TIME_SLEEP);
			io_write(CH_C, 1);
		}
		rt_task_wait_period(NULL);
	}
}

///////////////////
//////////DISTURBANCE ////
/////////////////
void* disturbance(){
	while(1){
		asm volatile("" ::: "memory");
	}
}
////////////////////
////////// MAIN ////////////
////////////////////

#define PERIODIC 1
#define DISTURBANCE 1
int main(int argc, char* argv[])
{
	mlockall(MCL_CURRENT | MCL_FUTURE);

	char  str[10];

	RT_TASK task_A_handle;
	RT_TASK task_B_handle;
	RT_TASK task_C_handle;

	pthread_t disturbances[10];

	printf("start task\n");
	//rt_task_create(&hello_task, str, 0, 50, T_CPU(1));
	rt_task_create(&task_A_handle, str, 0, 50, T_CPU(1));
	rt_task_create(&task_B_handle, str, 0, 50, T_CPU(1));
	rt_task_create(&task_C_handle, str, 0, 50, T_CPU(1));

	if(DISTURBANCE)
	{
		for(int i = 0; i<10; i++)
		{
			pthread_create(&disturbances[i], NULL, disturbance, NULL);
		}
	}

	if(PERIODIC){
		//rt_task_start(&hello_task, &helloWorld, 0);
		rt_task_start(&task_A_handle, &task_A_periodic, 0);
		rt_task_start(&task_B_handle, &task_B_periodic, 0);
		rt_task_start(&task_C_handle, &task_C_periodic, 0);
	} else {
		rt_task_start(&task_A_handle, &task_A_busy, 0);
		rt_task_start(&task_B_handle, &task_B_busy, 0);
		rt_task_start(&task_C_handle, &task_C_busy, 0);
	}

	if(DISTURBANCE) 
	{ 
		for(int i = 0; i <10; i++)
		{
			pthread_join(disturbances[i], NULL);
		}
	}

	while (1) {}
}
