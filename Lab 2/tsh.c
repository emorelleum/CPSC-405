/* 
 * tsh - A tiny shell program with job control
 * 
 * <Put your name and login ID here>
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include "util.h"
#include "jobs.h"


/* Global variables */
int verbose = 0;            /* if true, print additional output */

extern char **environ;      /* defined in libc */
static char prompt[] = "tsh> ";    /* command line prompt (DO NOT CHANGE) */
static struct job_t jobs[MAXJOBS]; /* The job list */
/* End global variables */


/* Function prototypes */

/* Here are the functions that you will implement */
void eval(char *cmdline);
int builtin_cmd(char **argv);
void do_bgfg(char **argv);
void waitfg(pid_t pid);

void sigchld_handler(int sig);
void sigtstp_handler(int sig);
void sigint_handler(int sig);

/* Here are helper routines that we've provided for you */
void usage(void);
void sigquit_handler(int sig);
int isnumeric(char *input);

/*
 * main - The shell's main routine 
 */
int main(int argc, char **argv){
	char c;
	char cmdline[MAXLINE];
	int emit_prompt = 1; /* emit prompt (default) */

	/* Redirect stderr to stdout (so that driver will get all output
	 * on the pipe connected to stdout) */
	dup2(1, 2);

	/* Parse the command line */
	while ((c = getopt(argc, argv, "hvp")) != EOF) {
		switch (c) {
			case 'h':             /* print help message */
				usage();
				break;
			case 'v':             /* emit additional diagnostic info */
				verbose = 1;
				break;
			case 'p':             /* don't print a prompt */
				emit_prompt = 0;  /* handy for automatic testing */
				break;
			default:
				usage();
		}
	}

	/* Install the signal handlers */

	/* These are the ones you will need to implement */
	Signal(SIGINT,  sigint_handler);   /* ctrl-c */
	Signal(SIGTSTP, sigtstp_handler);  /* ctrl-z */
	Signal(SIGCHLD, sigchld_handler);  /* Terminated or stopped child */
	
	/* This one provides a clean way to kill the shell */
	Signal(SIGQUIT, sigquit_handler); 

	/* Initialize the job list */
	initjobs(jobs);

	/* Execute the shell's read/eval loop */
	while (1) {

		/* Read command line */
		if (emit_prompt) {
			printf("%s", prompt);
			fflush(stdout);
		}
		if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin))
			app_error("fgets error");
		if (feof(stdin)) { /* End of file (ctrl-d) */
			fflush(stdout);
			exit(0);
		}

		/* Evaluate the command line */
		eval(cmdline);
		fflush(stdout);
		fflush(stdout);
	} 

	exit(0); /* control never reaches here */
}

/* 
 * eval - Evaluate the command line that the user has just typed in
 * 
 * If the user has requested a built-in command (quit, jobs, bg or fg)
 * then execute it immediately. Otherwise, fork a child process and
 * run the job in the context of the child. If the job is running in
 * the foreground, wait for it to terminate and then return.  Note:
 * each child process must have a unique process group ID so that our
 * background children don't receive SIGINT (SIGTSTP) from the kernel
 * when we type ctrl-c (ctrl-z) at the keyboard.  
 */
void eval(char *cmdline){
	char *argv[MAXARGS]; /* Argument list execve() */
	char buf[MAXLINE]; /* Holds modified command line */
	sigset_t mask;
	int bg; /* Should the job run in bg or fg? */
	pid_t pid; /* Process id */
	strcpy(buf, cmdline);
	bg = parseline(buf, argv);
	if (argv[0] == NULL)
		return; /* Ignore empty lines */

	if (!builtin_cmd(argv)) {
		sigemptyset(&mask);
		sigaddset(&mask, SIGCHLD);
		sigprocmask(SIG_BLOCK, &mask, NULL);
		if ((pid = fork()) == 0) { /* Child runs user job */
			setpgid(0, 0);
			sigprocmask(SIG_UNBLOCK, &mask, NULL);
			if (execve(argv[0], argv, environ) < 0) {
				printf("%s: Command not found.\n", argv[0]);
				return;
			}
		}
		/* Parent waits for foreground job to terminate */
		if (!bg) {
			addjob(jobs, pid, 1, cmdline);
			sigprocmask(SIG_UNBLOCK, &mask, NULL);
			waitfg(pid);
		}
		else{
			addjob(jobs, pid, 2, cmdline);
			printf("[%d] (%d) %s", maxjid(jobs), pid, cmdline);
		}
	}
	return;
}

/* 
 * builtin_cmd - If the user has typed a built-in command then execute
 *    it immediately.  
 * Return 1 if a builtin command was executed; return 0
 * if the argument passed in is *not* a builtin command.
 */

/* If first arg is a builtin command, run it and return true */
int builtin_cmd(char **argv){
	if (!strcmp(argv[0], "quit")){ /* quit command */
		exit(0);
		return 1;
	}
	if(!strcmp(argv[0], "jobs")){
		listjobs(jobs);		
		return 1;
	}
	if(!strcmp(argv[0], "bg") || !strcmp(argv[0], "fg")){
		do_bgfg(argv);
		return 1;
	}
	if(!strcmp(argv[0], "&")){ /* Ignore singleton & */
		return 1;
	}
	return 0; /* Not a builtin command */
}


/* 
 * do_bgfg - Execute the builtin bg and fg commands
 */
void do_bgfg(char **argv){
	if(argv[1] != NULL){ 
		struct job_t *job = NULL;
		char *jobinfo = argv[1];
		int isnumber, valid = 1;
		if(jobinfo[0] == '%'){
			memmove(jobinfo, jobinfo+1, strlen(jobinfo));//trims off leading % character
			if((isnumber = isnumeric(jobinfo)) == 1){
				int jid = atoi(jobinfo);
				job = getjobjid(jobs, jid);
				if(job == NULL){
					valid = 0;
					printf("%s: No such job\n", argv[1]); 
				}
			}
		}
		else{
			if((isnumber = isnumeric(jobinfo)) == 1){
				int pid = atoi(argv[1]);
				job = getjobpid(jobs, pid);
				if(job == NULL){
					valid = 0;
					printf("(%d): No such process\n", pid);
				}
			}
		}
		if(isnumber && valid){

			if(!strcmp(argv[0], "bg")){
				job->state = 2;
				printf("[%d] (%d) %s", job->jid, job->pid, job->cmdline);
				kill(-(job->pid), SIGCONT);	 
			}
			else if(!strcmp(argv[0], "fg")){
				if(job->state == 2){
					kill(-(job->pid), SIGSTOP);
				}
				job->state = 1;
				kill(-(job->pid), SIGCONT);
				waitfg(job->pid);	
			}
		}
		else if(!isnumber){
			printf("%s: argument must be a PID or %cjobid\n", argv[0], 0x25);
		}
	}
	else{
		printf("%s command requires PID or %cjobid argument\n", argv[0], 0x25);
	}
}

/* 
 * waitfg - Block until process pid is no longer the foreground process
 */
void waitfg(pid_t pid){
	while (fgpid(jobs)== pid){
		sleep(1);
	}
}

/*****************
 * Signal handlers
 *****************/

/* 
 * sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
 *     a child job terminates (becomes a zombie), or stops because it
 *     received a SIGSTOP or SIGTSTP signal. The handler reaps all
 *     available zombie children, but doesn't wait for any other
 *     currently running children to terminate.  
 */
void sigchld_handler(int sig){
	int status = 1;
	pid_t pid = waitpid(-1, &status, WNOHANG|WUNTRACED);
	if(pid){
		struct job_t *job = getjobpid(jobs, pid);
		int jobid = job->jid;
		if(WIFSTOPPED(status) && WSTOPSIG(status) == 20){
			printf("Job [%d] (%d) stopped by signal %d\n", jobid, pid, WSTOPSIG(status));
			job->state = 3;
		}
		else if(WIFSIGNALED(status)){
			printf("Job [%d] (%d) terminated by signal %d\n", jobid, pid, WTERMSIG(status));
			deletejob(jobs, pid);
		}
		else if(WIFEXITED(status)){
			deletejob(jobs, pid);
		}

	}
}

/* 
 * sigint_handler - The kernel sends a SIGINT to the shell whenver the
 *    user types ctrl-c at the keyboard.  Catch it and send it along
 *    to the foreground job.  
 */
void sigint_handler(int sig){	
	pid_t pid = fgpid(jobs);
	kill(-pid, sig);	
}

/*
 * sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
 *     the user types ctrl-z at the keyboard. Catch it and suspend the
 *     foreground job by sending it a SIGTSTP.  
 */
void sigtstp_handler(int sig){
	pid_t pid = fgpid(jobs);
	kill(-pid, sig);	
}

/*********************
 * End signal handlers
 *********************/



/***********************
 * Other helper routines
 ***********************/

/*
 * usage - print a help message
 */
void usage(void) {
	printf("Usage: shell [-hvp]\n");
	printf("   -h   print this message\n");
	printf("   -v   print additional diagnostic information\n");
	printf("   -p   do not emit a command prompt\n");
	exit(1);
}

/*
 * sigquit_handler - The driver program can gracefully terminate the
 *    child shell by sending it a SIGQUIT signal.
 */
void sigquit_handler(int sig){
	printf("Terminating after receipt of SIGQUIT signal\n");
	exit(1);
}

int isnumeric(char *input){
	int value = 1;
	int i = 0;
	while(i < strlen(input)){
		if(!isdigit(input[i])){
			value = 0;
		}
		i++;
	}
	return value;
}
