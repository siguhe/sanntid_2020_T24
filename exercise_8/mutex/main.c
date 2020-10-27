#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include <native/timer.h>

#include <sys/mman.h>
#include <sys/times.h>
#include <time.h>
#include <pthread.h>
#include <native/sem.h>
#include <native/mutex.h>
#include <native/task.h>
#include <rtdk.h>


#define MS100 100000000
#define S1 1000000000


#define PRIO_LOWEST 10
#define PRIO_MID 20
#define PRIO_HIGHEST 30
#define PRIO_MASTER 80


RT_MUTEX my_mut;
RT_SEM barrier;
static int finished = 0;


void cleanup(){
	rt_mutex_delete(&my_mut);
	rt_sem_delete(&barrier);
}

void task_A(void *arg) //Lowest priority
{
	rt_sem_p(&barrier, TM_INFINITE);
	rt_mutex_acquire(&my_mut, TM_INFINITE);

	rt_printf("Start A\n");
	rt_timer_spin(3*MS100);
	rt_printf("End A\n");
	rt_mutex_release(&my_mut);
	rt_sem_v(&barrier);
}

void task_B(void *arg) //Mid priority
{
	rt_sem_p(&barrier, TM_INFINITE);
	rt_task_sleep(MS100);

	rt_printf("Start B\n");
	rt_timer_spin(5*MS100);
	rt_printf("End B\n");
	rt_sem_v(&barrier);
}

void task_C(void *arg) //Highest priority
{
	rt_sem_p(&barrier, TM_INFINITE);
	rt_task_sleep(2*MS100);
	rt_mutex_acquire(&my_mut, TM_INFINITE);

	rt_printf("Start C\n");
	rt_timer_spin(2*MS100);
	rt_printf("End C\n");

	rt_mutex_release(&my_mut);

	rt_sem_v(&barrier);
}


void task_Master(void *arg)
{
	rt_printf("Before task Master\n");
	rt_task_sleep(MS100);
	rt_sem_broadcast(&barrier);
	rt_printf("In task Master\n");
	for(int i=0; i<10; i++){
		rt_task_sleep(S1);
	}


	cleanup();
	finished = 1;
	rt_printf("We are finished\n");
}

int main(int argc, char* argv[])
{
	char str[10];
	mlockall(MCL_CURRENT | MCL_FUTURE);
	rt_print_auto_init(1);

	rt_mutex_create(&my_mut, "Mutex");
	rt_sem_create(&barrier, "", 0, S_PRIO);

	RT_TASK task_A_handle;
	RT_TASK task_B_handle;
	RT_TASK task_C_handle;
	RT_TASK task_Master_handle;


	rt_task_create(&task_A_handle, str, 0, PRIO_LOWEST, T_CPU(1));
	rt_task_create(&task_B_handle, str, 0, PRIO_MID, T_CPU(1));
	rt_task_create(&task_C_handle, str, 0, PRIO_HIGHEST, T_CPU(1));
	rt_task_create(&task_Master_handle, str, 0, PRIO_MASTER, T_CPU(1));

	rt_task_start(&task_A_handle, task_A, 0);
	rt_task_start(&task_B_handle, task_B, 0);
	rt_task_start(&task_C_handle, task_C, 0);
	rt_task_start(&task_Master_handle, task_Master, 0);


	while (!finished) { }

	rt_printf("Exiting program....\n");

}
