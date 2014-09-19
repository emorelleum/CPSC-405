/*Hello.c -- Simple multi-threaded program.
  *
  * Compile with
  *    > gcc -g -Wall -Werror -D_POSIX_THREAD_SEMANTICS threadHello2.c -c -o threadHello2.o
  *    > gcc -g -Wall -Werror -D_POSIX_THREAD_SEMANTICS sthread.c -c -o sthread.o
  *    > gcc -lpthread threadHello2.o sthread.o -o threadHello2
  * Run with
  *    > ./threadHello
  */
#include <stdio.h>
#include <time.h>
#include "sthread.h"

static void go(int n);

#define NTHREADS 1000
pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

static sthread_t threads[NTHREADS];

int main(int argc, char **argv)
{
	clock_t start, finish;
	
int ii;
	start = clock();
	for(ii = 0; ii < NTHREADS; ii++){
		sthread_create(&(threads[ii]), &go, ii);
	}
	for(ii = 0; ii < NTHREADS; ii++){
		sthread_join(threads[ii]);
		//long ret = sthread_join(threads[ii]);
  		//printf("Thread %d returned %ld\n", ii, ret);
	}
	finish = clock();
	printf("Time: %f seconds\n", (double)(finish - start) / CLOCKS_PER_SEC);
	return 0;
}

void go(int n){
	//printf("Hello from thread %d\n", n);
	sthread_exit(0);
}
