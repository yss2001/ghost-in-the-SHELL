#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<stdlib.h>
#include<string.h>
#include<sys/file.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<fcntl.h>

#include "pipe.h"
#include "ls.h"
#include "cd.h"
#include "pinfo.h"
#include "history.h"
#include "echo.h"
#include "pwd.h"
#include "redirection.h"

int pipe_fd[2];
pid_t proc_id;
int inp_fd = 0;

int check_pipe(char command[])
{
    int num_of_pipes=0;

    for(int i=0; i<strlen(command); i++)
    {
        if(command[i] == '|')
            num_of_pipes += 1;
    }
    
    if(num_of_pipes==0)
        return 0;
    else 
        return num_of_pipes+1;
}

void parse_pipe(char command[], int num_of_pipes)
{
    inp_fd=0;
    int index=0;
    for(int i=0; i<35000; i++)
        args[i] = NULL;

    int i=0, j=0, pipe_part=0;

    while(i<strlen(command))
    {
        //tokenized by |
        j=0;
        while(command[i]!='|' && i<strlen(command))
        {
            part[j] = command[i];
            i++, j++;
        }
        part[j] = '\0';
        j=j-1;

        while(part[j]==' ')
            j=j-1;
        
        part[j+1] = '\0';
        
        check_redirection(part);
        //now each individual command split into arguments
        int k=0;
        index=0;
        while(k<strlen(part))
        {
            j=0;
            while(part[k]!=' ' && k<strlen(part))
            {
                part2[j] = part[k];
                j++, k++;
            }
            part2[j] = '\0';
            
            if(!strcmp("<", part2) || !strcmp(">", part2) || !strcmp(">>", part2))
                break;

            args[index] = NULL;
            args[index] = (char *)malloc(sizeof(char)*(strlen(part2)+1));
            for(int a=0; a<strlen(part2); a++)
            {
                args[index][a] = part2[a];
            }
            args[index][strlen(part2)] = '\0';
            index = index + 1;

            while(part[k]==' ' && k<strlen(part))
                k++;
        }
        
        //each part of pipe sent for execution
        args[index] = NULL;
        pipe_part += 1;
        if(pipe_part==num_of_pipes)
            pipe_part = -1;
        execute_pipe(args, pipe_part);

        i+=1;
        while(command[i]==' ' && i<strlen(command))
            i+=1;
        
        for(int z=0; z<index; z++)
        {
            free(args[z]);
        }
    }
}

void execute_pipe(char **command, int pipe_part)
{
    int insave = dup(0);
    int outsave = dup(1);
    if(pipe(pipe_fd)<0)
    {
        perror("Pipe Error: ");
        return;
    }
        
    proc_id = fork();

    if(proc_id<0)
    {
        perror("Fork Error: ");
        return;
    }
    else if(proc_id==0)
    {
        dup2(inp_fd, 0);
        if(pipe_part!=-1)
            dup2(pipe_fd[1], 1);
        close(pipe_fd[0]);
        if(!strcmp("ls", command[0]))
        {
            open_redirection_files();
            ls(part);
            exit(1);
        }
        else if(!strcmp("cd", command[0]))
        {
            open_redirection_files();
            cd(part);
            exit(1);
        }
        else if(!strcmp("pinfo", command[0]))
        {
            open_redirection_files();
            pinfo(part);
            exit(1);
        }
        else if(!strcmp("history", command[0]))
        {
            open_redirection_files();
            print_history(part);
            exit(1);
        }
        else if(!strcmp("echo", command[0]))
        {
            open_redirection_files();
            echo(part);
            exit(1);
        }
        else if(!strcmp("pwd", command[0]))
        {
            open_redirection_files();
            pwd_print(part);
            exit(1);
        }
        else 
        {
            open_redirection_files();   
            execvp(command[0], command);
        }
    }
    else
    {
        int status=0;
        int finished = waitpid(proc_id, &status, WUNTRACED);

        while(!WIFEXITED(status) && !WIFSIGNALED(status))
            finished = waitpid(proc_id, &status, WUNTRACED);
            
        close(pipe_fd[1]);
        inp_fd = pipe_fd[0];
        
        fflush(stdout);
        fflush(stdin);
        dup2(insave, 0);
        dup2(outsave, 1);
        close(insave);
        close(outsave);
    }
}
