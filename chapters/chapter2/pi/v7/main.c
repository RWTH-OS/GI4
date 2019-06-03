#include <stdalign.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define MAX_THREADS		2

alignas(16) double four[] = {4.0, 4.0};
alignas(16) double two[] = {2.0, 2.0};
alignas(16) double one[] = {1.0, 1.0};
alignas(16) double ofs[] = {0.5, 1.5};

long long num_steps = 1000000;
double step;

typedef struct {
	double sum;
	long long start, end;
} thread_param;

extern void calcPi_SSE_thread(long long, long long, double *);

void *thread_func(void *arg)
{
	thread_param *thr_arg = (thread_param *) arg;
	double sum = 0.0;

	calcPi_SSE_thread(thr_arg->start, thr_arg->end, &sum);
	thr_arg->sum = sum;

	return 0;
}

int main(int argc, char **argv)
{
	double sum;
	int i;
	struct timeval start, end;
	pthread_t threads[MAX_THREADS];
	thread_param thr_arg[MAX_THREADS];

	if (argc > 1)
		num_steps = atoll(argv[1]);
	if (num_steps < 100)
		num_steps = 1000000;

	printf("\nnum_steps = %lld\n", num_steps);

	gettimeofday(&start, NULL);

	sum = 0.0;
	step = 1.0 / (double)num_steps;

	/* Create MAX_THREADS worker threads. */
	for (i = 0; i < MAX_THREADS; i++) {
		/* initialize arguments of the thread  */
		thr_arg[i].start = i * (num_steps / MAX_THREADS);
		thr_arg[i].end = (i + 1) * (num_steps / MAX_THREADS);
		thr_arg[i].sum = 0.0;
		pthread_create(&(threads[i]), NULL, thread_func, &(thr_arg[i]));
	}

	/* Wait until all threads have terminated
	   and calculate sum*/
	for (i = 0; i < MAX_THREADS; i++) {
		pthread_join(threads[i], NULL);
		sum += thr_arg[i].sum;
	}

	gettimeofday(&end, NULL);

	printf("PI = %f\n", sum * step);
	printf("Time : %lf sec\n", (double)(end.tv_sec-start.tv_sec)+(double)(end.tv_usec-start.tv_usec)/1000000.0);

	return 0;
}
