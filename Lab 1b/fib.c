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
	pid_t pid1; //process id1
	pid_t pid2; //process id2
	int p1[2];
	int p2[2];
	int status1;
	int status2;
	int *sum = 0;
	if(n <=  1){
		if(doPrint){
			printf("%d\n", n);
			
		}
		sum = n;
		printf("%d", sum);//tests the sum
		write(p1[1], sum, 8);//writes sum to pipe 1 ***THESE ARE THE PROBLEM****
		write(p2[1], sum, 8);//writes sum to pipe 2 ***THESE ARE THE PROBLEM****
		exit(sum); //returns the sum as the exit status
	}

	if((pid1 = fork())==0){ //forks once to do n-1 recurion
		pipe(p1);//I think you need to pipe each time and then the recursion will cause writing to the pipe, then you read in this if
		close(p1[0]); 
		doFib(n-1, 0);
		//sleep(1);
		int num2; 
		//read(p1[0], num2, 8);
		//printf("%d", num2);
		sum += num2;
		write(p1[1], sum, 8);//writes the new sum to pipe
		exit(sum);
	}
	if((pid2 = fork())==0){ //forks second time to do n-2 recursion
		pipe(p2);
		close(p2[0]); 
		//printf("%d", num);
		doFib(n-2, 0);
		//sleep(1);
		int num;
		//read(p2[0], num, 8);
		sum += num;
		write(p2[1], sum, 8);//writes new sum to pipe
		exit(sum);

	}
	
	waitpid(pid1, &status1, 0); //parent process waits til this finishes
	waitpid(pid2, &status2, 0); //parent process waits til this finishes
	if(WIFEXITED(status1) && WIFEXITED(status2)){ //if both exited succesfully
		int sum2;
		read(p1[0], sum, 8); //gets the exit status from both processes and gets their sum
		read(p2[0], sum2, 8);
		sum += sum2;
		if(doPrint){
			printf("%d\n", sum); //prints final answer if doPrint is 1
		}
	}
	else{
		printf("error");
	}
	exit(sum);
}


