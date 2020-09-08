#include <stdio.h>
#include <time.h>
#include <sys/times.h>
#include "x86intrin.h"
#include "sched.h"

#define CLK_FREQ 2660000000L

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
	printf("now: utime=%d, stime=%d, return=%d\n", tmp.tms_utime, tmp.tms_stime, start_time);
	start_time = times(&tmp);
	printf("now: utime=%d, stime=%d, return=%d\n", tmp.tms_utime, tmp.tms_stime, start_time);
	while(elapsed_time < start_time + sec*100){
		for(int i = 0; i < 10000; i++){}
		elapsed_time = times(&tmp);
	}
}

void histogram_rdtsc(){
	int ns_max = 50;
	int histogram[ns_max];

	memset(histogram, 0, sizeof(int)*ns_max);

	for(int i = 0; i < 10*1000*1000; i++){
		unsigned long long t1 = __rdtsc();
		unsigned long long t2 = __rdtsc();

		int ns = ((t2 - t1) * CLK_FREQ) / 1000000000L;

		if (ns >= 0 && ns < ns_max){
			histogram[ns]++;
		}
	}
	
	for(int i = 0; i < ns_max; i++){
		printf("%d\n", histogram[i]);
	}

}


void histogram_gettime(){
	int ns_max = 50;
	int histogram[ns_max];

	memset(histogram, 0, sizeof(int)*ns_max);

	for(int i = 0; i < 10*1000*1000; i++){
		struct timespec now, then;
		clock_gettime(CLOCK_MONOTONIC, &now);
		clock_gettime(CLOCK_MONOTONIC, &then);

		struct timespec diff = timespec_sub(then, now);
		int ns = diff.tv_nsec;

		if (ns >= 0 && ns < ns_max){
			histogram[ns]++;
		}
	}
	
	for(int i = 0; i < ns_max; i++){
		printf("%d\n", histogram[i]);
	}
}


void histogram_gettime_switch()
{
	int ns_max = 500;
	int histogram[ns_max];

	memset(histogram, 0, sizeof(int)*ns_max);

	for(int i = 0; i < 10*1000*1000; i++){
		struct timespec now, then;
		clock_gettime(CLOCK_MONOTONIC, &now);
		sched_yield();
		clock_gettime(CLOCK_MONOTONIC, &then);

		struct timespec diff = timespec_sub(then, now);
		int ns = diff.tv_nsec;

		if (ns >= 0 && ns < ns_max){
			histogram[ns]++;
		}
	}

	for(int i = 0; i < ns_max; i++){
		printf("%d\n", histogram[i]);
	}
}

void histogram_times(){
	int ns_max = 50;
	int histogram[ns_max];

	memset(histogram, 0, sizeof(int)*ns_max);

	for(int i = 0; i < 10*1000*1000; i++){
		struct tms start = {0};
		struct tms stop = {0};
		unsigned long long start_time = times(&start);
		unsigned long long stop_time = times(&stop);

		unsigned long long diff_dsec = stop.tms_utime - start.tms_utime;
		//printf("times diff: %llu\n", diff);
		int ns = diff_dsec * 100 / 1000000000; // TODO

		if (ns >= 0 && ns < ns_max){
			histogram[ns]++;
		}
	}
	
	for(int i = 0; i < ns_max; i++){
		printf("%d\n", histogram[i]);
	}

}

int main(int argc, char *argv[])
{
	struct timespec t1, t2;
	struct tms times_s = {0};
	unsigned long long ticks = 0;

	t1.tv_sec = 1;
	t1.tv_nsec = 0;

	t1.tv_sec = 0;
	t2.tv_nsec = 0;

	//sleep(1);
	//usleep(1000000);
	//nanosleep(&t1, &t2);

	//busy_wait(t1);
	//busy_wait_times(1);
	//long long n = 10*1000*1000;
	//for(int i = 0; i < n; i++){	
		//__rdtsc();
		//clock_gettime(CLOCK_MONOTONIC,&t2);
		//times(&times_s);
	//}

	//histogram_rdtsc();
	//histogram_gettime();
	//histogram_times();

 	histogram_gettime_switch();

	return 0;
}

