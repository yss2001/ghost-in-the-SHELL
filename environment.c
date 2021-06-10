#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<stdlib.h>
#include<string.h>
#include<sys/file.h>
#include<stdlib.h>

#include "environment.h"

void set_variable(char command[4000])
{
    int i=6, j=0, arg_pos=0;
    char part[400], var[400], content[400];

    if(strlen(command)<=7)
    {
        printf("Arguments not specified!\n");
        return;
    }

    //parsed input for arguments
    while(i<strlen(command))
    {
        while(command[i]==' ' && i<strlen(command))
            i++;
        
        j=0;
        while(command[i]!=' ' && i<strlen(command))
        {
            part[j] = command[i];
            j++, i++;
        }
        part[j] = '\0';
        arg_pos+=1;

        if(arg_pos>2)
        {
            printf("Too many arguments specified!\n");
            return;
        }

        if(arg_pos==1)
            strcpy(var, part);
        else if(arg_pos==2)
            strcpy(content, part);
    }

    if(arg_pos<2)
    {
        printf("Too few arguments specified!\n");
        return;
    }

    if(setenv(var, content, 1)<0)
        printf("Setting failed!\n");

}

void unset_variable(char command[])
{
    int i=8, j=0, arg_pos=0;
    char part[400], var[400];

    if(strlen(command)<=9)
    {
        printf("Arguments not specified!\n");
        return;
    }

    //parsed input for arguments
    while(i<strlen(command))
    {
        while(command[i]==' ' && i<strlen(command))
            i++;
        
        j=0;
        while(command[i]!=' ' && i<strlen(command))
        {
            part[j] = command[i];
            j++, i++;
        }
        part[j] = '\0';
        arg_pos+=1;

        if(arg_pos>1)
        {
            printf("Too many arguments specified!\n");
            return;
        }

        if(arg_pos==1)
        {
            strcpy(var, part);
        }
    }

    if(unsetenv(var)<0)
        printf("Unsetting failed!\n");
}