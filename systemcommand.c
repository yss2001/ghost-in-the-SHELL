#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<stdlib.h>
#include<string.h>
#include<sys/wait.h>
#include<signal.h>
#include<sys/file.h>

#include "systemcommand.h"
#include "redirection.h"

//boolean for handling of CtrlC
int kill_ins=0;
//boolean for handling of CtrlZ
int send=0;


extern char prompt_line[4000];

int background=0;
int process_name_index=0;

void print_termination(int pid)
{
    int index=0;
    //printed process name by comparing process ids
    for(int i=0; i<process_name_index; i++)
    {
        if(pid == process_names[i].process_id)
        {
            printf("\n%s ", process_names[i].process_name);
            index = i;
            break;
        }
    }

    for(int i=index; i<process_name_index-1; i++)
    {
        process_names[i].process_id = process_names[i+1].process_id;
        strcpy(process_names[i+1].process_name, process_names[i].process_name);
    }

    process_name_index = process_name_index - 1;                                                                                

    printf("with pid %d has exited\n", pid);
}

void handler(int sig)
{
    int status; 
    pid_t pid;
    do
    {
        //waited until the child has either exited or been stopped or been terminated
        int status;
        pid_t pid = waitpid(-1, &status, WNOHANG);
        if(pid <= 0)
        {
            break;
        }
        if(WIFEXITED(status) || WIFSIGNALED(status) || WIFSTOPPED(status))
        {
            print_termination(pid);
            write(1, "\033[1;36m", strlen("\033[1;36m"));
            write(1, prompt_line, strlen(prompt_line));
            write(1, "\033[0m", strlen("\033[0m"));
        }

    }while (!WIFEXITED(status) && !WIFSIGNALED(status));
}

void send_to_bg(int sig)
{
    send = 1;
}


void kill_fg(int sig)
{
    kill_ins=1;
}

void system_command(char command[])
{
    kill_ins = 0;
    send = 0;
    signal(SIGINT, kill_fg);
    signal(SIGTSTP, send_to_bg);
    int red_begins=0;
    background=0;
    for(int i=0; i<35000; i++)
        arguments[i] = NULL;
    
    //system command name found
    char command_name[50];
    int i=0;
    while(command[i]!= ' ' && i<strlen(command))
    {
        command_name[i] = command[i];
        i++;
    }
    command_name[i] = '\0';

    int start = strlen(command_name) + 1;
    char arg[300] = "\0";
    int j=0, k=1;
    arguments[0] = (char *)malloc(sizeof(char)*(strlen(command_name)+1));

    for(int i=0; i<strlen(command_name); i++)
        arguments[0][i] = command_name[i];
    arguments[0][strlen(command_name)] = '\0';

    //all arguments stored in an array along with command name
    while(start < strlen(command))
    {
        j=0;
        while(command[start]!= ' ' && (start<strlen(command)))
        {
            arg[j] = command[start];
            start++, j++;
        }
        arg[j] = '\0';

        if(!strcmp(">>", arg) || !strcmp(">", arg) || !strcmp("<", arg))
            red_begins = 1;
        
        if(strcmp("&", arg) && red_begins==0)
        {
            arguments[k] = (char *)malloc(sizeof(char)*(strlen(arg)+1));
            for(int i=0; i<strlen(arg); i++)
                arguments[k][i] = arg[i];
            arguments[k][strlen(arg)] = '\0';
            k = k+1;
        }
        else if(!strcmp("&", arg))
            background = 1;

        while(command[start]==' ' && (start<strlen(command)))
            start++;
    }
    
    arguments[k] = NULL;

    if(background)
    {
        strcpy(process_names[process_name_index].process_name, arguments[0]);
    }

    //parent forked into child
    int fork_id = fork();
    int status;
    int did_child_finish;

    if(fork_id < 0)
    {
        perror("Error: ");
        return;
    }
    if(fork_id == 0)
    {
        if(background)
            setpgid(0,0);
        
        open_redirection_files();
        //ran child process
        if(execvp(arguments[0], arguments)==-1)
        {
            printf("Invalid command!\n");
            exit(0);
        }
    }
    else
    {
        if(background)
        {
            process_names[process_name_index].process_id = fork_id;
            process_name_index += 1;
        }
        //waiting until foreground process is done
        if(!background)
        {
            did_child_finish = waitpid(fork_id, &status, WUNTRACED);

            while(!WIFEXITED(status) && !WIFSIGNALED(status))
            {
                if(kill_ins)
                {
                    kill(fork_id, 9);
                    printf("\n");
                }

                if(send)
                {
                    kill(fork_id, 20);
                    process_names[process_name_index].process_id = fork_id;
                    strcpy(process_names[process_name_index].process_name, arguments[0]);
                    process_name_index += 1;
                    signal(SIGCHLD, handler);
                    printf("\n");
                    break;
                }
                did_child_finish = waitpid(fork_id, &status, WUNTRACED);
            }

        }
        else 
        {
            printf("Began process with pid %d\n", fork_id);
            signal(SIGCHLD, handler);
        }

        //freed memory used for storing arguments
        for(int i=0; i<k; i++)
        {
            free(arguments[i]);
        }
    }

    
}
