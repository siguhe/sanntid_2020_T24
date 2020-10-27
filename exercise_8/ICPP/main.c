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


#define PRIO_LOWEST 1
#define PRIO_MID 2
#define PRIO_HIGHEST 3
#define PRIO_CEILING 50
#define PRIO_MASTER 80


RT_SEM barrier;
static int finished = 0;

struct ICPP_mut {
	RT_MUTEX mutex;
	int priority;
	int taken;
};

struct Task_prio {
	RT_TASK task;
	int original;
	int current;
};

static struct Task_prio task_A_prio = {0};
static struct Task_prio task_B_prio = {0};

static struct ICPP_mut mut_A = {0};
static struct ICPP_mut mut_B = {0};


void cleanup(){
	rt_mutex_delete(&mut_B.mutex);
	rt_mutex_delete(&mut_A.mutex);
	rt_sem_delete(&barrier);
}

void icpp_lock( struct Task_prio *task_prio, struct ICPP_mut *mut)
{

	rt_timer_spin(1000);
	if (task_A_prio.current == PRIO_CEILING ||
			task_B_prio.current == PRIO_CEILING) {
		// nothing
		rt_printf("Highest prio already taken!\n");
	} else {
		rt_printf("This task gets highest prio!\n");
		task_prio->current = PRIO_CEILING;
		rt_task_set_priority(&(task_prio->task), task_prio->current);
	}

	if (task_prio->current == PRIO_CEILING) {
		rt_printf("We acquire the mutex yey!\n");
		rt_mutex_acquire(&mut->mutex, TM_INFINITE);	
	} else {
		rt_printf("We wait for the mutex yey!\n");

		// Wait until highest priority task is done
		while (task_A_prio.current == PRIO_CEILING || task_B_prio.current == PRIO_CEILING);

		rt_mutex_acquire(&mut->mutex, TM_INFINITE);	
		rt_printf("After ages we get the mutex!\n");

	}

}

void icpp_unlock( struct Task_prio *task_prio, struct ICPP_mut *mut){
	rt_mutex_release(&mut->mutex);
	task_prio->current = task_prio->original;
	rt_printf("Mutex released!\n");
	rt_task_set_priority(&task_prio->task, task_prio->original);
}


void task_A(void *arg) //Lowest priority
{
	rt_sem_p(&barrier, TM_INFINITE);
	//	rt_mutex_acquire(&mut_A.mutex, TM_INFINITE);
	icpp_lock(&task_A_prio, &mut_A);
	{

		rt_printf("Start A-1\n");
		rt_timer_spin(3*MS100);
		rt_printf("End A-1\n");

		//		rt_mutex_acquire(&mut_B.mutex, TM_INFINITE);
		icpp_lock(&task_A_prio, &mut_B);
		{

			rt_printf("Start A-2\n");
			rt_timer_spin(3*MS100);
			rt_printf("End A-2\n");
		}	
		//		rt_mutex_release(&mut_B.mutex);
		icpp_unlock(&task_A_prio, &mut_B);
	}
	//	rt_mutex_release(&mut_A.mutex);
	icpp_unlock(&task_A_prio, &mut_A);

	rt_printf("Start A-3\n");
	rt_timer_spin(1*MS100);
	rt_printf("End A-3\n");


	rt_sem_v(&barrier);
}

void task_B(void *arg) //Mid priority
{
	rt_sem_p(&barrier, TM_INFINITE);

	rt_task_sleep(MS100);
	//	rt_mutex_acquire(&mut_B.mutex,TM_INFINITE);
	icpp_lock(&task_B_prio, &mut_B);
	{
		rt_printf("Start B-1\n");
		rt_timer_spin(1*MS100);
		rt_printf("End B-1\n");

		//		rt_mutex_acquire(&mut_A.mutex,TM_INFINITE);
		icpp_lock(&task_B_prio, &mut_A);
		{
			rt_printf("Start B-2\n");
			rt_timer_spin(2*MS100);
			rt_printf("End B-2\n");
		}
		//		rt_mutex_release(&mut_A.mutex);
		icpp_unlock(&task_B_prio, &mut_A);
	}
	//	rt_mutex_release(&mut_B.mutex);
	icpp_unlock(&task_B_prio, &mut_B);


	rt_printf("Start B-3\n");
	rt_timer_spin(1*MS100);
	rt_printf("End B-3\n");

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

	RT_TASK task_A_handle;
	RT_TASK task_B_handle;
	RT_TASK task_Master_handle;


	task_A_prio.task = task_A_handle;
	task_A_prio.original = PRIO_LOWEST;
	task_A_prio.current = PRIO_LOWEST;

	task_B_prio.task = task_B_handle;
	task_B_prio.original = PRIO_MID;
	task_B_prio.current = PRIO_MID;

	rt_mutex_create(&mut_A.mutex, str);
	rt_mutex_create(&mut_B.mutex, str);

	mut_A.priority = PRIO_HIGHEST;
	mut_B.priority = PRIO_HIGHEST;
	mut_A.taken = 0;
	mut_B.taken = 0;
	rt_sem_create(&barrier, "", 0, S_PRIO);


	rt_task_create(&task_A_handle, str, 0, task_A_prio.original, T_CPU(1));
	rt_task_create(&task_B_handle, str, 0, task_B_prio.original, T_CPU(1));
	rt_task_create(&task_Master_handle, str, 0, PRIO_MASTER, T_CPU(1));

	rt_task_start(&task_A_handle, task_A, 0);
	rt_task_start(&task_B_handle, task_B, 0);
	rt_task_start(&task_Master_handle, task_Master, 0);


	while (!finished) { }

	rt_printf("Exiting program....\n");

}
