#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/file.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<string.h>

#include "pinfo.h"

void pinfo_print(char pid[])
{
    char executable_path[10000];
    pid_t process_id;
    char path_input[50];

    //created the path for opening
    if(!strcmp("cur", pid))
    {
        process_id = getpid();
        sprintf(path_input, "/proc/%d/stat", process_id);
        printf("pid -- %d\n", process_id);
    }
    else
    {
        strcpy(path_input, "/proc/");
        strcat(path_input, pid);
        strcat(path_input, "/stat");

        printf("pid -- %s\n", pid);
    }

    int stat_file = open(path_input, O_RDONLY);

    if(stat_file<0)
    {
        printf("No such process exists!\n");
        return;
    }
    int number_of_spaces = 0;

    char c, vsize[20];
    int k=0;

    //iterated through stat file to print status and size
    while(read(stat_file, &c, 1))
    {
        if(c==' ')
                number_of_spaces+=1;

            if(number_of_spaces==2 && c!=' ')
                printf("process status -- %c\n", c);

            if(number_of_spaces==22)
            {
                vsize[k] = c;
                k++;
            }

            if(number_of_spaces>22)
            {
                vsize[k] = '\0';
                break;
            } 
    }

    printf("memory -- %s\n", vsize);
    
    //created the path for finding exectuable's path
    if(!strcmp("cur", pid))
        sprintf(path_input, "/proc/%d/exe", process_id);
    else
    {
        strcpy(path_input, "/proc/");
        strcat(path_input, pid);
        strcat(path_input, "/exe");
    }

    //readlink used to find executable's path
    int read = readlink(path_input, executable_path, sizeof(executable_path));
    executable_path[read] = '\0';
    printf("executable path -- %s\n", executable_path);
    
    close(stat_file);

}

void pinfo(char command[])
{
    char input_pid[50];
    int called = 0;
    //command parsed for finding pids
    int start = 6, i=0;
    while(start < strlen(command))
    {
        while(command[start]==' ' && start<strlen(command))
            start++;
                
        i = 0;
        while(command[start]!=' ' && start < strlen(command))
        {
            input_pid[i] = command[start];
            i++;
            start++;
        }
        input_pid[i] = '\0';

        if(!strcmp(input_pid, ">") || !strcmp(input_pid, "<") || !strcmp(input_pid, ">>"))
            break;
            
        if(input_pid[0]!='\0')
            pinfo_print(input_pid), called=1;

        if(start!=strlen(command))
            printf("\n");
    }
        if(!called)
            pinfo_print("cur");
    
}