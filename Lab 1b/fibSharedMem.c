#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>

const int MAX = 13;
#define SHMSZ     1024
static void doFib(int n, int doPrint);

/*
 * unix_error - unix-style error routine.
 */
inline static void 
unix_error(char *msg)
{
	fprintf(stdout, "%s: %s\n", msg, strerror(errno));
	exit(1);
}


int main(int argc, char **argv)
{
	int arg;
	int print;

	if(argc != 2){
		fprintf(stderr, "Usage: fib <num>\n");
		exit(-1);
	}

	if(argc >= 3){
		print = 1;
	}

	arg = atoi(argv[1]);
	if(arg < 0 || arg > MAX){
		fprintf(stderr, "number must be between 0 and %d\n", MAX);
		exit(-1);
	}
	int *result[13];
	int shmid;
	key_t key;
	key = 1234;
	if ((shmid = shmget(key, MAX*sizeof(int), IPC_CREAT | 0666)) < 0){
		unix_error("ERROR SHMGET");
	}
	int x;
	for(x = 0; x < 13; x++){  
		if ((result[x] = (int *)shmat(shmid, NULL, 0)) == (int *) -1){
			unix_error("ERROR SHMAT");
		}
	}
	for(x = 0; x < 13; x++){
		*result[x] = 0;
	}
	doFib(arg, 1);

	return 0;
}

/* 
 * Recursively compute the specified number. If print is
 * true, print it. Otherwise, provide it to my parent process.
 *
 * NOTE: The solution must be recursive and it must fork
 * a new child for each call. Each process should call
 * doFib() exactly once.
 */
static void doFib(int n, int doPrint){
	int shmid;
 	key_t key;
 	pid_t pid;
	int status;
	int *result;
	key = 1234;
	if ((shmid = shmget(key, MAX*sizeof(int), 0666)) < 0){
		unix_error("ERROR SHMGET");
	}
	int x;
	for(x = 0; x < 13; x++){  
		if ((result = (int *)shmat(shmid, NULL, 0)) == (int *) -1){
			unix_error("ERROR SHMAT");
		}
	}
	if(n < 2){
		if(doPrint){
			printf("%d\n", n);
		}
		else{
  	     		result[n] = n;
		}
		exit(0);
	}
	pid = fork();
	if(pid == 0){
		doFib(n-1, 0);
		//*result[n] = *result[n-1] + *result[n-2];
	}
	waitpid(pid, &status, 0); //parent process waits til this finishes
	result[n] = (result[n-1]) + (result[n-2]);
	if(doPrint){
		printf("%d\n", result[n]); //prints final answer if doPrint is 1
	}
	exit(0);
}
