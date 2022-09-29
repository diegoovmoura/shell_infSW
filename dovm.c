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
    char args[MAX_LINE_LENGTH];	// command line has max of 40 arguments
    char save[MAX_LINE_LENGTH]; // copty of the command line for the history
    int has_save = 0;
    int should_run = 1, verify_style = 1;
    pid_t pid;
    FILE *file;
     
    if (argc > 2)
    {
        printf("Numero de parametros incorreto!\n");
    }
    else if (argc == 2) // BATCH
    {
        char *command_args[40] = {0};
        file = fopen(argv[1], "r");

        // if the file does not open
        if (file == NULL)
        { 
            printf("erro ao abrir o arquivo!\n");
            return 0;
        }

        int count = 0;
        while(fgets(args, MAX_LINE_LENGTH, file)) // read the file
        {
            printf("%s",args);
            // removing the \n and \r in the final of string
            for (int i = 0; i < strlen(args); i++)
            {
                if (args[i] == '\n' || args[i] == '\r')
                {
                    args[i] = 0;
                }    
            }
            
            // saving and alocating the string
            command_args[count] = malloc(MAX_LINE_LENGTH);
            strcpy(command_args[count], args);

            if (strcmp(command_args[count], "exit") == 0) { //exit
                break;
            }    
            count++;    
        }

        for (int i = 0; i < count; i++)
        {
            // sequential mode
            if (verify_style == 1)
            {       
                if (strcmp(command_args[i], "!!") == 0) { //history
                    if (has_save == 0) // check if has a command in history
                    {
                        printf("No commands\n");
                        continue;
                    }                 
                    run_sequential(save, pid); // passing the previous command line
                    continue;
                }
                if (strcmp(command_args[i], "style parallel") == 0) { //change style
                    verify_style = 0;
                    continue;
                }
                run_sequential(command_args[i], pid);
                if (strlen(command_args[i]) > 1)
                {
                    strcpy(save, command_args[i]);
                    has_save = 1;
                }   
            }
            // parallel mode
            else
            {
                if (strcmp(command_args[i], "!!") == 0) { //history
                    if (has_save == 0) // check if has a command in history
                    {
                        printf("No commands\n");
                        continue;
                    }
                    run_sequential(save, pid); // passing the previous command line
                    continue;
                }
                if (strcmp(command_args[i], "style sequential") == 0) { //change style
                    verify_style = 1;
                    continue;
                } 
                run_parallel(command_args[i], pid);
                if (strlen(command_args[i]) > 1)
                {
                    strcpy(save, command_args[i]);
                    has_save = 1;
                }   
            }
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
                
                if (strcmp(args, "!!") == 0)
                {        
                    if (has_save == 0) // check if has a command in history
                    {
                        printf("No commands\n");
                    }                
                    run_sequential(save, pid);
                    continue;
                }          
                if (strcmp(args, "exit") == 0) // exit 
                {  
                    break;
                }
                else if (strcmp(args, "style parallel") == 0) //changing style
                {  
                    verify_style = 0;
                    continue;
                }
                if (strlen(args) > 1)
                {
                    strcpy(save, args);
                    has_save = 1;
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
                
                if (strcmp(args, "!!") == 0)
                {
                    if (has_save == 0) // check if has a command in history
                    {
                        printf("No commands\n");
                    }                
                    run_parallel(save, pid);
                    continue;
                }        
                if (strcmp(args, "exit") == 0) { //exit
                    break;
                } 
                if (strcmp(args, "style sequential") == 0) {  //changing style 
                    verify_style = 1;
                    continue;
                } 
                if (strlen(args) > 1)
                {
                    strcpy(save, args);
                    has_save = 1;
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
    int lock = 0;
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
            // taking off unecessaries spaces
            if (i == 0 && slice[i] == ' ')
            {
                continue;
            }
            if (slice[i] == ' ' && slice[i - 1] != ' ' && slice[i+1] == ' ')
            {
                continue;
            }
            
            else if (i > 0 && slice[i - 1] == ' ' && slice[i] == ' ')
            {
                continue;
            }

            // if space or NULL found, assign NULL into aux
            else if(slice[i]==' '||slice[i]=='\0')
            {
                aux[count][j]='\0';
                count++;  //for next command
                j=0;    //for set no aruguments
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
            if (execvp(command_args[0], command_args) == -1)
            {
                printf("%s: command not found\n", command_args[0]);
            }
            exit(pid);
        }

        slice = strtok(NULL, ";");
    }
}


void run_parallel(char args[40], pid_t pid)
{
    int a = 0, lock = 0;
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
            // taking off unecessaries spaces
            if (i == 0 && slice[i] == ' ')
            {
                continue;
            }
            if (slice[i] == ' ' && slice[i - 1] != ' ' && slice[i+1] == ' ')
            {
                continue;
            }
            
            else if (i > 0 && slice[i - 1] == ' ' && slice[i] == ' ')
            {
                continue;
            }

            // if space or NULL found, assign NULL into aux
            else if(slice[i]==' '||slice[i]=='\0')
            {
                aux[count][j]='\0';
                count++;  //for next command
                j=0;    //for set no aruguments
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
            if (execvp(command_args[0], command_args) == -1)
            {
                printf("%s: command not found\n", command_args[0]);
            }
            exit(pid);
        }
        slice = strtok(NULL, ";");
        a++; // amount of slices
    }
    for (int i = 0; i < a; i++)
    {
        if (pid > 0) // shell waits for the end of the commands processes
        {
            wait(NULL);
        }
    }
}