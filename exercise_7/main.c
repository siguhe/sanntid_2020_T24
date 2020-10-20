#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include <native/task.h>
//#include <native/times.h>
#include <sys/mman.h>


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



int main(int argc, char* argv[])
{

	char  str[10];
	mlockall(MCL_CURRENT | MCL_FUTURE);

	printf("start task\n");
	//sprintf(str,"hello");

	/* Create task
	 * Arguments: &task,
	 *            name,
	 *            stack size (0=default),
	 *            priority,
	 *            mode (FPU, start suspended, ...)
	 */

	rt_task_create(&hello_task, str, 0, 50, T_CPU(1));

	/*  Start task
	 * Arguments: &task,
	 *            task function,
	 *            function argument
	 */
	rt_task_start(&hello_task, &helloWorld, 0);

	while (1) {}

}
