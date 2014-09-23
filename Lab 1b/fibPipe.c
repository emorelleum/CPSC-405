#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

const int MAX = 13;
static void doFib(int n, int doPrint);

 //unix_error - unix-style error routine.
inline static void unix_error(char *msg){
    fprintf(stdout, "%s: %s\n", msg, strerror(errno));
    exit(1);
}
pid_t Fork(void){
	pid_t pid;
	if ((pid = fork()) < 0)
	        unix_error("Fork Error");
	return pid;
}

int main(int argc, char **argv){
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
	pid_t pid1;
	pid_t pid2;
	int status;
	int fd[2];

	int *sum = 0, *num = 0;
	int x = 0, y = 0;
	sum = &x;
	num = &y;		
	if(n <=  1){ //0 or 1
		(*sum) = n;
	}
	else{
		if(pipe(fd) == -1){
			unix_error("Pipe Error");
		}  // create the pipe

		pid1 = Fork();
		if(pid1==0){
			doFib(n-1, fd[1]);
		}
		pid2 = Fork();
		if(pid2==0){
			doFib(n-2, fd[1]);
		}
		waitpid(pid1, &status, 0);
		waitpid(pid2, &status, 0);
		close(fd[1]);
		int i = 0;
		for(i = 0; i < 2; i++){
			read(fd[0], num, sizeof(int));
			(*sum) += (*num);
		}
	}	
	if(doPrint == 1){
		printf("%d\n", (*sum));
	}
	else{
		write(doPrint, sum, sizeof(int));
		exit(0);
	}
}
