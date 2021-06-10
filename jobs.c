#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/file.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<string.h>
#include<signal.h>
#include<sys/wait.h>

#include "jobs.h"
#include "systemcommand.h"

extern int process_name_index;
extern char prompt_line[4000];
int ctrlc = 0, ctrlz = 0;

void exit_handler(int sig)
{
    int status;

    do
    {
        pid_t pid = waitpid(-1, &status, WNOHANG);
        if(pid<=0)
            break;
        
        if(WIFEXITED(status) || WIFSIGNALED(status) || WIFSTOPPED(status))
        {
            print_termination(pid);
            write(1, "\033[1;36m", strlen("\033[1;36m"));
            write(1, prompt_line, strlen(prompt_line));
            write(1, "\033[0m", strlen("\033[0m"));
        }
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    
    
}
void ctrlc_func(int sig)
{
    ctrlc = 1;
}

void ctrlz_func(int sig)
{
    ctrlz = 1;
}
void change_state(char command[], int type)
{
    signal(SIGINT, ctrlc_func);
    signal(SIGTSTP, ctrlz_func);
    ctrlc = 0, ctrlz = 0;
    char path_input[50];
    int proc_id = 0;
    if(command[0] == 'f')
        type = 0;
    else if(command[1] == 'b')
        type = 1;
    
    proc_id = atoi(command+3);
    if(proc_id < 0 || proc_id > (process_name_index+1))
    {
        printf("No such process exists!\n");
        return;
    }

    strcpy(path_input, "/proc");
    sprintf(path_input, "/proc/%d/stat", process_names[proc_id-1].process_id);

    int stat_file = open(path_input, O_RDONLY);

    if(stat_file<0)
    {
        printf("Error: Couldn't open stat file!\n");
    }

    int number_of_spaces = 0;
    char c;

    while(read(stat_file, &c, 1))
    {
        if(c==' ')
            number_of_spaces+=1;
            
        if(number_of_spaces==2 && c!=' ')
            break;
    }
    
    //bg implemented
    if(type==1)
    {
        if(c=='R')
            return;
        else
        {
            kill(process_names[proc_id-1].process_id, SIGCONT);
            signal(SIGCHLD, exit_handler);
        } 
    }
    
    //fg implemented
    if(type==0)
    {
        kill(process_names[proc_id-1].process_id, SIGCONT);
        
        int id = process_names[proc_id-1].process_id;
        char com[30];
        strcpy(com, process_names[proc_id-1].process_name);

        for(int i=proc_id-1; i<process_name_index-1; i++)
        {
            process_names[i].process_id = process_names[i+1].process_id;
            strcpy(process_names[i+1].process_name, process_names[i].process_name);
        }

        process_name_index = process_name_index - 1;

        int wait_status=0;
        waitpid(id, &wait_status, WUNTRACED);

        while(!WIFEXITED(wait_status) && !WIFSIGNALED(wait_status))
        {
            if(ctrlc)
            {
                kill(id, 9);
                printf("\n");
            }

            if(ctrlz)
            {
                kill(id, 20);
                process_names[process_name_index].process_id = id;
                strcpy(process_names[process_name_index].process_name, com);
                process_name_index += 1;
                printf("\n");
                break;
            }

            waitpid(id, &wait_status, WUNTRACED);
        }

    }

    close(stat_file);


}
void print_jobs()
{
    char path_input[50];
    for(int i=0; i<process_name_index; i++)
    {
        strcpy(path_input, "/proc/");
        sprintf(path_input, "/proc/%d/stat", process_names[i].process_id);
        printf("[%d] ", i+1);

        int stat_file = open(path_input, O_RDONLY);

        if(stat_file<0)
        {
            printf("Error: Couldn't open stat file!\n");
            continue;
        }

        int number_of_spaces = 0;
        char c;

        while(read(stat_file, &c, 1))
        {
            if(c==' ')
                number_of_spaces+=1;
            
            if(number_of_spaces==2 && c!=' ')
                break;
        }

        if(c=='R')
            printf("%s ", "Running");
        else
            printf("%s ", "Stopped");
        
        close(stat_file);

        printf("%s [%d]\n", process_names[i].process_name, process_names[i].process_id);
    }
}