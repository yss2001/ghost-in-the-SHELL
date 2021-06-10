#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<stdlib.h>
#include<string.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<signal.h>

#include "systemcommand.h"
#include "jobkill.h"

extern int process_name_index;

void kjob(char command[])
{
    int process_num=0, signal_num=0;
    pid_t pid_to_kill=0;
    char char_process_num[5], signal_char[3];

    int start=4, j=0;

    while(command[start]==' ' && start<strlen(command))
        start++;

    j=0;
    while(command[start]!=' ' && start<strlen(command))
    {
        char_process_num[j] = command[start];
        start++, j++;
    }
    char_process_num[j] = '\0';

    process_num = atoi(char_process_num);

    if(process_num<=0 || process_num>(process_name_index+1))
    {
        printf("No such process exists!\n");
        return;
    }

    while(command[start]==' ' && start<strlen(command))
        start++;
    
    signal_char[0] = command[start];
    signal_char[1] = command[start+1];
    signal_char[2] = '\0';

    signal_num = atoi(signal_char);

    pid_to_kill = process_names[process_num-1].process_id;

    if(kill(pid_to_kill, signal_num)<0)
    {
        printf("No such process exists!\n");
        return;
    }
}

void overkill()
{
    for(int i=0; i<process_name_index; i++)
    {
        if(kill(process_names[i].process_id, 9)<0)
            printf("Termination failed for: %d\n", process_names[i].process_id);
    }
}