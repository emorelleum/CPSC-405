#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>


inline static void 

// these two procedures from book
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

// end of code from book

int main(int argc, char **argv)
{
  pid_t pid;
  int status;
  int bytes;
  int fd[2];    // pipe file descriptor                         
  char buffer[80];

  char pooh[] = "Good morning Pooh Bear";


  if (pipe(fd) == -1){
    unix_error("Pipe Error");
  }  // create the pipe

  
  pid = Fork();  // fork with error checking
  if (pid == 0){  /* child */
      // child closes input side of pipe
      close(fd[0]); 
      write(fd[1], pooh, (strlen(pooh)+1));
      exit(0);
  }
  else {  /* parent */
       // parent closes output side of pipe
       close(fd[1]);
       wait(&status);
       read(fd[0], buffer, sizeof(buffer));
       printf("Eeyore says: '%s'\n", buffer);
  }
  

  return 0;
}

