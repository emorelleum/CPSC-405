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
static void eeyore();

inline static void 
unix_error(char *msg)
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

int main(int argc, char **argv)
{
  int *result;
  int  shmid;
  pid_t pid;
  int status;
  key_t key;

  key = 1234;
  if ((shmid = shmget(key, MAX*sizeof(int), IPC_CREAT | 0666)) < 0)
    unix_error("ERROR SHMGET");
  if ((result = (int *)shmat(shmid, NULL, 0)) == (int *) -1)
    unix_error("ERROR SHMAT");
  
  *result = 0;
  printf("Value before forking child: %d\n", *result);

  pid = Fork();
  if (pid == 0)  /* child */
      eeyore();
  else {  /* parent */
       wait(&status);
       printf("Value after child exits: %d\n", *result);
  }
  

  return 0;
}


static void eeyore() {

  int shmid;
  key_t key;
  key = 1234;
  int *result;
  if ((shmid = shmget(key, MAX*sizeof(int), 0666)) < 0)
    unix_error("ERROR SHMGET");
  if ((result = (int *)shmat(shmid, NULL, 0)) == (int *) -1)
    unix_error("ERROR SHMAT");

  *result = 7;
  printf ("Value in child: %d \n", *result);
}
