#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <time.h>


void sigint_handler(int s){
	size_t bytes;
	const int STDOUT = 1;
	bytes = write(STDOUT, "Nice try.\n", 10); 
	if(bytes != 10){
		exit(-999);	
	}
}

void sigusr1_handler(int s){
	size_t bytes;
	const int STDOUT = 1;
	bytes = write(STDOUT, "exiting\n", 8);
	if(bytes != 8){
		exit(-999);
	}
	else{ 
		exit(1);
	}
}

int main(int argc,char** argv){
	printf("%d\n", getpid());
	if (signal(SIGINT, sigint_handler) == SIG_ERR)
		printf("signal error: \n");

	if (signal(SIGUSR1, sigusr1_handler) == SIG_ERR)
		printf("signal error: \n");
	
	while(1){
		struct timespec tim, tim2;
		tim.tv_sec = 1;
		printf("Still here\n");
		nanosleep(&tim, &tim2);
	}
	return 0;
}
