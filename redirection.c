#include<string.h>
#include<sys/file.h>
#include<stdio.h>
#include<unistd.h>

#include "redirection.h"

int red_in = 0, red_out=0, red_out_app=0;
char red_in_file[300], red_out_file[300];
int inpfile, outfile;
void check_redirection(char command[])
{
    red_in=0, red_out=0, red_out_app=0;

    int i=0, j=0;
    char part[400];

    while(i<strlen(command))
    {
        j=0;
        while(command[i]!=' ' && i<strlen(command))
        {
            part[j] = command[i];
            j++, i++;
        }

        part[j] = '\0';

        if(!strcmp(part, "<"))
            red_in = 2;
        else if(!(strcmp(part, ">")))
            red_out = 2;
        else if(!strcmp(part, ">>"))
            red_out_app = 2;
        
        if(red_in==2 && strcmp(part, "<"))
        {
            strcpy(red_in_file, part);
            red_in = red_in - 1;
        }

        if(red_out==2 && strcmp(part, ">"))
        {
            strcpy(red_out_file, part);
            red_out = red_out - 1;
        }

        if(red_out_app==2 && strcmp(part, ">>"))
        {
            strcpy(red_out_file, part);
            red_out_app = red_out_app - 1;
        }

        while(command[i]==' ' && i<strlen(command))
            i++;
    }
}

void open_redirection_files()
{
    if(red_in)
    {
        inpfile = open(red_in_file, O_RDONLY);
        if(inpfile<0)
        {
            printf("Input file doesn't exist!\n");
            return;
        }

        if(dup2(inpfile, 0)<0)
        {
            perror("Error: ");
            return;
        }

        close(inpfile);
    }

    if(red_out)
    {
        outfile = open(red_out_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if(outfile<0)
            return;
        
        if(dup2(outfile, 1)<0)
        {
            perror("Error: ");
            return;
        }

        close(outfile);
    }

    if(red_out_app)
    {
        outfile = open(red_out_file, O_WRONLY | O_APPEND, 0644);
        if(outfile<0)
            return;
        
        if(dup2(outfile, 1)<0)
        {
            perror("Error: ");
            return;
        }

        close(outfile);
    }
}