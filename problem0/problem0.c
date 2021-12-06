#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

	float*				local_pi;
	int 				n, threads_num;

void * adder (void * args) {

	int 				i, first_i;
	float				len, local_sum, x;

	local_sum = 0.0;
	len = 1.0 / (float) n;
	first_i = *(int *) args;

	for (i = first_i; i <= n; i += threads_num) {
		x = len * ((float)i - 0.5);
		local_sum += (4.0 / (1.0 + x*x));
	}

	local_pi[first_i - 1] = len * local_sum;
	return 0;
}

int main (int agrc, char * argv[]) {

	int 				i;
	struct timeval 		tv0, tv1;
	float				pi = 0.0;

	if (gettimeofday(&tv0, 0)) {
		fprintf(stderr, "Error getting timeval\n");
    	return 1;
	};

	n = atol(argv[1]);
	threads_num = atol(argv[2]);
	printf("Conditions: %d partition intervals, %d threads\n", n, threads_num);

	local_pi = (float *)malloc(sizeof(float)*threads_num);
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

  	for (i = 0; i < threads_num; i++) {
  		pi += local_pi[i];
  	}
  	free(local_pi);
  	printf ("pi = %.6f\n", pi);
  	if (gettimeofday(&tv1, 0)) {
  		fprintf(stderr, "Error getting timeval\n");
    	return 1;
	}
	float computing_time = (float)(tv1.tv_sec-tv0.tv_sec) + (float)(tv1.tv_usec-tv0.tv_usec)/1000000;
  	printf("Elapsed time: %.6f s\n", computing_time);
  	return 0;
}
