#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_LINE_LENGTH 41

void run_sequential(char args[MAX_LINE_LENGTH], pid_t pid);
void run_parallel(char args[MAX_LINE_LENGTH], pid_t pid);

int main(int argc, char *argv[])
{
    char args[MAX_LINE_LENGTH];	/* command line has max of 40 arguments */
    int should_run = 1, verify_style = 1;
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
        while (should_run) 
        {
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
                run_sequential(args, pid);

            }
            // parallel mode
            else
            {
                printf("dovm par> ");
                fflush(stdout);

                fgets(args, sizeof(args), stdin);
                args[strlen(args) - 1] = 0;
                
                if (strcmp(args, "exit") == 0) { //exit
                    return 0;
                } 
                if (strcmp(args, "style sequential") == 0) {  //changing style 
                    verify_style = 1;
                    continue;
                } 

                // runing the command line in terminal
                run_parallel(args, pid);      
            }  
        }
    }
	return 0;
}

void run_sequential(char args[MAX_LINE_LENGTH], pid_t pid)
{
    // slicing the line of commands
    char *slice = strtok(args, ";");  
    while (slice != NULL)
    {
        char *command_args[20] = {0};
        char aux[20][20] = {0};
        
        // slicing again to take the command arguments
        int count = 0, j = 0;
        for (int i = 0; i < strlen(slice); i++)
        {
            // if space or NULL found, assign NULL into aux
            if(slice[i]==' '||slice[i]=='\0')
            {
                aux[count][j]='\0';
                count++;  //for next word
                j=0;    //for next word, init index to 0
            }
            else
            {
                aux[count][j]=slice[i];
                j++;
            }
        }   
        for (int i = 0; i < count +1; i++)
        {
            command_args[i] = aux[i];   
        }

        pid = fork();

        if (pid > 0) // shell waits for the end of the commands processes
        {
            wait(NULL);
        }
        else // run the command in terminal and ends the child process
        {
            execvp(command_args[0], command_args);
            exit(pid);
        }

        slice = strtok(NULL, ";");
    }
}

void run_parallel(char args[40], pid_t pid)
{
    int a = 0;

    // slicing the line of commands
    char *slice = strtok(args, ";");
    while (slice != NULL)
    {
        char *command_args[20] = {0};
        char aux[20][20] = {0};
        
        // slicing again to take the command arguments
        int count = 0, j = 0;
        for (int i = 0; i < strlen(slice); i++)
        {
            // if space or NULL found, assign NULL into aux
            if(slice[i]==' '||slice[i]=='\0')
            {
                aux[count][j]='\0';
                count++;  //for next word
                j=0;    //for next word, init index to 0
            }
            else
            {
                aux[count][j]=slice[i];
                j++;
            }
        }   
        for (int i = 0; i < count +1; i++)
        {
            command_args[i] = aux[i];   
        }

        pid = fork();

        if (pid == 0) // run the command in terminal and ends the child process
        {
            execvp(command_args[0], command_args);
            exit(pid);
        }
        slice = strtok(NULL, ";");
        a++;
    }
    for (int i = 0; i < a; i++)
    {
        if (pid > 0) // shell waits for the end of the commands processes
        {
            wait(NULL);
        }
    }
}