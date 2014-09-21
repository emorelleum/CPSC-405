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
inline static void unix_error(char *msg)
{
    fprintf(stdout, "%s: %s\n", msg, strerror(errno));
    exit(1);
}
pid_t Fork(void) {
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
	int status1;
	int status2;
	int fd[2];
	if(pipe(fd) == -1){
		unix_error("Pipe Error");
	}  // create the pipe
	

	int *sum = 0;		
	if(n <=  1){
		printf("%d\n", n); //tests output
		read(fd[0], sum, sizeof(int)); //reads from pipe
		int x = (*sum) + n;
		sum = &x; //increments pipe input value by n
		
		close(fd[0]);
		write(fd[1], sum, sizeof(int));	 //writes to pipe output
		exit(0);
	}
	pid1 = Fork();
	if(pid1==0){
		doFib(n-1, 0);
		exit(0);
	}
	pid2 = Fork();
	if(pid2==0){
		doFib(n-2, 0);
		exit(0);
	}

	waitpid(pid1, &status1, 0);
	waitpid(pid2, &status2, 0);
	if(doPrint){
		printf("doPrint");
		read(fd[1], &sum, sizeof(int));
		printf("%d", (*sum));
	}	
	exit(0);
}
