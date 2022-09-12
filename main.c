#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_LINE 80 /* 80 chars per line, per command */

void run_sequential(char args[40], char *slice, pid_t pid);
void run_parallel(char args[40], char *slice, pid_t pid);

int main(int argc, char *argv[])
{
    char args[MAX_LINE/2 + 1];	/* command line has max of 40 arguments */
    int should_run = 1, verify_style = 1;
    char *slice;
    pid_t pid;
    FILE *file;

    if (argc > 1) // BATCH
    {
        file = fopen(argv[1], "r");

        if (file == NULL)
        {
            printf("error");
            return 0;
        }
        
    }  
    else // INERATIVE
    {
        while (should_run) {

            // sequential mode
            if (verify_style == 1)
            {
                printf("dovm seq> ");
                fflush(stdout);

                fgets(args, sizeof(args), stdin);
                args[strlen(args) - 1] = 0;
                
                if (strcmp(args, "exit") == 0) // exit 
                {  
                    return 0;
                }
                if (strcmp(args, "style parallel") == 0) //changing style
                {  
                    verify_style = 0;
                    continue;
                } 

                // runing the command line in terminal
                run_sequential(args, slice, pid);

            }
            // parallel mode
            else
            {
                printf("dovm par> ");
                fflush(stdout);

                fgets(args, sizeof(args), stdin);
                args[strlen(args) - 1] = 0;
                
                if (strcmp(args, "exit") == 0) {
                    return 0;
                } 
                if (strcmp(args, "style sequential") == 0) {  
                    verify_style = 1;
                    continue;
                } 

                // runing the command line in terminal
                run_parallel(args, slice, pid);      
            }  
        }
    }
	return 0;
}

void run_sequential(char args[40], char *slice, pid_t pid){
    slice = strtok(args, ";");
    while (slice != NULL)
    {
        pid = fork();

        if (pid > 0) // shell waits for the end of the commands processes
        {
            wait(NULL);
        }
        else // run the command in terminal and ends the child process
        {
            system(slice);
            exit(pid);
        }
        slice = strtok(NULL, ";");
    }
}

void run_parallel(char args[40], char *slice, pid_t pid){
    int count = 0;

    // slicing the line of commands
    slice = strtok(args, ";");
    while (slice != NULL)
    {
        count++;
        pid = fork();

        if (pid == 0) // run the command in terminal and ends the child process
        {
            system(slice);
            exit(pid);
        }
        slice = strtok(NULL, ";");
    }
    for (int i = 0; i < count; i++)
    {
        if (pid > 0) // shell waits for the end of the commands processes
        {
            wait(NULL);
        }
    }
}