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

const int MAX = 15;
#define SHMSZ     1024
static void phy(int n, int doPrint);

/*
 * unix_error - unix-style error routine.
 */
inline static void 
unix_error(char *msg){
	fprintf(stdout, "%s: %s\n", msg, strerror(errno));
	exit(1);
}


int main(int argc, char **argv){
	int arg;

	if(argc != 2){
		fprintf(stderr, "Usage: fib <num>\n");
		exit(-1);
	}

	arg = atoi(argv[1]);
	if(arg < 0 || arg > MAX){
		fprintf(stderr, "number must be between 0 and %d\n", MAX);
		exit(-1);
	}
	int *result[15];
	int shmid;
	key_t key;
	key = 1234;
	if ((shmid = shmget(key, MAX*sizeof(int), IPC_CREAT | 0666)) < 0){
		unix_error("ERROR SHMGET");
	}
	int x;
	for(x = 0; x < 15; x++){  
		if ((result[x] = (int *)shmat(shmid, NULL, 0)) == (int *) -1){
			unix_error("ERROR SHMAT");
		}
	}
	for(x = 0; x < 15; x++){
		*result[x] = 0;
	}
	phy(arg, 1);

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
static void phy(int n, int doPrint){
	int shmid;
 	key_t key;
 	pid_t pid1;
 	pid_t pid2;
	int status;
	int *result;
	key = 1234;
	if ((shmid = shmget(key, MAX*sizeof(int), 0666)) < 0){
		unix_error("ERROR SHMGET");
	}
	int x;
	for(x = 0; x < 15; x++){  
		if ((result = (int *)shmat(shmid, NULL, 0)) == (int *) -1){
			unix_error("ERROR SHMAT");
		}
	}
	if(n < 3){
		int phylNum;
		if(n < 2){
			phylNum = n;
		}
		else{
			phylNum = 1;
		}
		if(doPrint){
			printf("%d\n", phylNum);
		}
		else{
  	     		result[n] = phylNum;
		}
		exit(0);
	}
	pid1 = fork();
	if(pid1 == 0){
		phy(n-1, 0);
	}
	pid2 = fork();
	if(pid2 == 0){
		phy(n-2, 0);
	}

	waitpid(pid1, &status, 0); //parent process waits til this finishes
	waitpid(pid2, &status, 0); //parent process waits til this finishes
	result[n] = (result[n-1]) + (result[n-2] + result[n-3]);
	if(doPrint){
		printf("%d\n", result[n]); //prints final answer if doPrint is 1
	}
	exit(0);
}
