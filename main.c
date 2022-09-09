#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_LINE 80 /* 80 chars per line, per command */

int main(int argc, char *argv[])
{
    char args[MAX_LINE/2 + 1];	/* command line has max of 40 arguments */
    int should_run = 1;		/* flag to help exit program*/
    pid_t pid;
    
    if (argc <= 1)
    {
        while (should_run) {
            printf("dovm seq> ");
            fflush(stdout);

            fgets(args, sizeof(args), stdin);
            args[strlen(args) - 1] = 0;
            
            if (strcmp(args, "exit") == 0) {  
                return 0;
            } 

            pid = fork();
            if (pid > 0)
            {
                wait(NULL);
            }
            else
            {
                system(args);
                exit(pid);
            }
            
            /*
            After read the input, the next steps are:
                - create n child process with fork() command
                - the child process will invoke execvp()
                - parent will invoke wait() unless command line include &	 
            */
        }
        /* code */
    } else {
        while (should_run) {
            printf("dovm par> ");
            fflush(stdout);

            fgets(args, sizeof(args), stdin);
            args[strlen(args) - 1] = 0;
            
            if (strcmp(args, "exit") == 0) {
                return 0;
            } 

            pid = fork();
            if (pid > 0)
            {
                wait(NULL);
            }
            else
            {
                system(args);
                exit(pid);
            }
            
            /*
            After read the input, the next steps are:
                - create n child process with fork() command
                - the child process will invoke execvp()
                - parent will invoke wait() unless command line include &	 
            */
        }
    }
    
	return 0;
}