#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<stdlib.h>
#include<string.h>
#include<sys/file.h>

#include "echo.h"
#include "redirection.h"

extern int red_in, red_out, red_out_app;

extern char red_in_file[300], red_out_file[300];

void echo(char command[])
{
    //starting set to argument of command
    int start=4;
    while(command[start]==' ' || command[start]=='\t')
        start++;
    
    int qoute_number = 0, space=0;

    for(int i=start; i<strlen(command); i++)
    {
        if(command[i]=='>' || command[i]=='<')
            break;
        
        if((i+1)<strlen(command) && (command[i]=='>' && command[i+1]=='>'))
            break;
        //number of qoutations stored
        if(command[i]=='\"')
        {
            qoute_number = (qoute_number+1)%2;
        }
        
        //whether \ is to be escaped or not
        if(command[i]=='\\')
        {
            if(qoute_number==1)
                printf("%c", command[i]);
        }
        
        else if(command[i]!='\"')
        {
            //whitespaces inside argument taken care of
            if(command[i]==' ' || command[i]=='\t')
            {
                if(space==0)
                {
                    printf("%c", command[i]);
                    space = 1;
                }
            }
            else
            {
                space = 0;
                printf("%c", command[i]);
            }
        }
        
    }
    if(red_out==0 && red_out_app==0)
        printf("\n");
}