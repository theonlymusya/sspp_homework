#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

	double				pi = 0.0;
	int 				n, threads_num;

void * adder (void * args) {

	int 				i, first_i;
	double				len, sum, x;

	sum = 0.0;
	len = 1.0 / (double) n;
	first_i = *(int *) args;

	for (i = first_i; i <= n; i += threads_num) {
		x = len * ((double)i - 0.5);
		sum += (4.0 / (1.0 + x*x));
	}

	pi += len * sum;
	return 0;
}

int main (int agrc, char * argv[]) {

	int 				i;
	struct timeval 		tv0, tv1;

	if (gettimeofday(&tv0, 0)) {
		fprintf(stderr, "Error getting timeval\n");
    	return 1;
	};

	n = atol(argv[1]);
	threads_num = atol(argv[2]);
	printf("Conditions: %d partition intervals, %d threads\n", n, threads_num);

	for (i = 1; i <= threads_num; i++) {
		pthread_t thread;
		int res = i;
  		if (pthread_create(&thread, NULL, adder, &res)) {
    		fprintf(stderr, "Error creating a new thread\n");
    		return 1;
  		}
  		if (pthread_join(thread, NULL)) {
    		fprintf(stderr, "Error joining the thread\n");
    		return 1;
    	}
  	}
  	printf ("pi = %.6f\n", pi);
  	if (gettimeofday(&tv1, 0)) {
  		fprintf(stderr, "Error getting timeval\n");
    	return 1;
	}
	double computing_time = (tv1.tv_sec-tv0.tv_sec) + (double)(tv1.tv_usec-tv0.tv_usec)/1000000;
  	printf("Elapsed time: %.6f s\n", computing_time);
  	return 0;
}