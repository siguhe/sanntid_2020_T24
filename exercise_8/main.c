#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include <native/timer.h>

#include <sys/mman.h>
#include <sys/times.h>
#include <time.h>
#include <pthread.h>
#include <native/sem.h>
#include <native/task.h>
#include <rtdk.h>


#define MS100 100000000
#define S1 1000000000

#define PRIO_LOWEST 10
#define PRIO_MID 20
#define PRIO_HIGHEST 30


RT_SEM my_sem;
static int finished = 0;


void cleanup(){
	rt_sem_delete(&my_sem);
}

void task_A(void *arg)
{
	rt_sem_p(&my_sem, TM_INFINITE);
	rt_printf("In task A with resource\n");
	rt_sem_v(&my_sem);
}

void task_B(void *arg)
{
	rt_sem_p(&my_sem, TM_INFINITE);
	rt_printf("In task B with resource\n");
	rt_sem_v(&my_sem);
}

void task_Master(void *arg)
{
	rt_printf("Before task Master\n");
	rt_task_sleep(MS100);
	rt_sem_broadcast(&my_sem);
	rt_printf("In task Master\n");
	rt_task_sleep(S1);



	cleanup();
	finished = 1;
	rt_printf("We are finished\n");
}

int main(int argc, char* argv[])
{
	char str[10];
	mlockall(MCL_CURRENT | MCL_FUTURE);
	rt_print_auto_init(1);

	rt_sem_create(&my_sem, "", 0, S_PRIO);

	RT_TASK task_A_handle;
	RT_TASK task_B_handle;
	RT_TASK task_Master_handle;


	rt_task_create(&task_A_handle, str, 0, PRIO_LOWEST, T_CPU(1));
	rt_task_create(&task_B_handle, str, 0, PRIO_MID, T_CPU(1));
	rt_task_create(&task_Master_handle, str, 0, PRIO_HIGHEST, T_CPU(1));

	rt_task_start(&task_A_handle, task_A, 0);
	rt_task_start(&task_B_handle, task_B, 0);
	rt_task_start(&task_Master_handle, task_Master, 0);


	while (!finished) { }

	rt_printf("Exiting program....\n");

}
