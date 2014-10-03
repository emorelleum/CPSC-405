#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <time.h>
#include "util.h"


void my_handler(int s){
	size_t bytes;
	const int STDOUT = 1;
	bytes = write(STDOUT, "Nice try.\n", 10); if(bytes != 10)
	fflush(stdout);	
}

int main(int argc,char** argv)
{

	struct sigaction old_action;

	old_action.sa_handler = my_handler;
	sigemptyset(&old_action.sa_mask);
	old_action.sa_flags = 0;

	sigaction(SIGINT, &old_action, NULL);
	while(1){
		struct timespec tim, tim2;
		tim.tv_sec = 1;
		printf("Still here\n");
		nanosleep(&tim, &tim2);
	}
	return 0;
}
