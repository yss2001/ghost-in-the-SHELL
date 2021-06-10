#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<stdlib.h>
#include<string.h>
#include<sys/file.h>
#include<stdlib.h>

#include "commandhandler.h"
#include "echo.h"
#include "pwd.h"
#include "cd.h" 
#include "ls.h"
#include "systemcommand.h"
#include "pinfo.h"
#include "history.h"
#include "nightswatch.h"
#include "redirection.h"
#include "environment.h"
#include "pipe.h"
#include "prompt.h"
#include "jobs.h"
#include "jobkill.h"

void commandhandler(char parameter[])
{
    int insave = dup(0);
    int outsave = dup(1);

    char issued[100000];
    strcpy(issued, parameter);
    start = 0, end = strlen(issued)-1;

    //trailing and leading whitespaces removed
    while(issued[start] == ' ' || issued[start] == '\t')
    {
        start++;
    }
    while(issued[end]== ' ' || issued[end] == '\t')
    {
        end--;
    }

    for(int i=0; i<(end-start+1); i++)
    {
        issued[i] = issued[i+start];
    }

    if(start!=0 || end!=strlen(issued)-1)
        issued[end-start+1]='\0';

    //redirections checked and files opened
    check_redirection(issued);

    //piping checked
    int is_pipe = check_pipe(issued);

    //control branched based on type of command
    char command_type[50];
    int i=0;
    while(issued[i]!= ' ' && i<strlen(issued))
    {
        command_type[i] = issued[i];
        i++;
    }
    command_type[i] = '\0';

    //shell quit
    if(!strcmp(command_type, "quit"))
    {
        exit(0);
    }
    else if(is_pipe)
        parse_pipe(issued, is_pipe);
    else if(!strcmp(command_type, "echo"))
    {
        open_redirection_files();
        echo(issued);
    }
    
    else if(!strcmp(command_type, "pwd"))
    {
        open_redirection_files();
        pwd_print();
    }
    
    else if(!strcmp(command_type, "cd"))
        cd(issued);
    
    else if(!strcmp(command_type, "ls"))
    {
        open_redirection_files();
        ls(issued);
    }
    else if(!strcmp(command_type, "pinfo"))
    {
        open_redirection_files();
        pinfo(issued);
    }
    else if(!strcmp(command_type, "history"))
    {
        open_redirection_files();
        print_history(issued);
    }
    else if(!strcmp(command_type, "nightswatch"))
        watch_printer(issued);
    else if(!strcmp(command_type, "setenv"))
        set_variable(issued);
    else if(!strcmp(command_type, "unsetenv"))
        unset_variable(issued);
    else if(!strcmp(command_type, "jobs"))
        print_jobs();
    else if(!strcmp(command_type, "kjob"))
        kjob(issued);
    else if(!strcmp(command_type, "overkill"))
        overkill();
    else if(!strcmp(command_type, "bg"))
        change_state(issued, 1);
    else if(!strcmp(command_type, "fg"))
        change_state(issued, 0);
    else
    {
        system_command(issued);
    }
    
    if(strcmp(command_type, "cd"))
        chdir(curdirectory);

    fflush(stdout);
    fflush(stdin);
    dup2(insave, 0);
    dup2(outsave, 1);
    close(insave);
    close(outsave);
}