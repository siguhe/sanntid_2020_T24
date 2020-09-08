#include <stdio.h>
#include <time.h>
#include <sys/times.h>

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


void busy_wait(struct timespec t){
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	struct timespec then = timespec_add(now, t);
	while(timespec_cmp(now, then) < 0){
		for(int i = 0; i < 10000; i++){}
		clock_gettime(CLOCK_MONOTONIC, &now);
	}
}

void busy_wait_times(int sec){
	struct tms tmp = {0};
	unsigned long long start_time = times(&tmp);
	unsigned long long elapsed_time = start_time;

	//printf("now: utime=%d, stime=%d, return=%d\n", tmp.tms_utime, tmp.tms_stime, start_time);
	while(elapsed_time < start_time + sec*100){
		for(int i = 0; i < 10000; i++){}
		elapsed_time = times(&tmp);
	}
}

int main(int argc, char *argv[])
{
	struct timespec t1, t2;
	struct tms times_s = {0};

	t1.tv_sec = 1;
	t1.tv_nsec = 0;

	t2.tv_nsec = 0;

	//sleep(1);
	//usleep(1000000);
	//nanosleep(&t1, &t2);
	
	//busy_wait(t1);
	busy_wait_times(1);


	return 0;
}
