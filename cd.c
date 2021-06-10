#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<stdlib.h>
#include<string.h>

extern char home_directory[4000];
extern char curdirectory[4000];
extern char prevdirectory[4000];
void cd(char command[])
{
    chdir(curdirectory);
    //for command 'cd'
    if(command[2]=='\0')
        chdir(home_directory);
    else
    {
        int start=3;
        while(command[start]==' ')
            start++;
        
        //removed redirection characters
        int end=strlen(command)-1;

        while(command[end]=='>' || command[end]=='<')
            end--;
        
        while(command[end]==' ')
            end--;
        
        if(end!=strlen(command))
            command[end+1] = '\0';
        
        //changed to gitsh's home if ~ is argument
        if(!strcmp("~", command+start))
        {
            chdir(home_directory);
        }
        else if(!strcmp("-", command+start))
        {
            chdir(curdirectory);
            chdir(prevdirectory);
        }
        else
        {
            chdir(curdirectory);
            if(chdir(command+start)<0)
            {
                perror("Error");
                chdir(curdirectory);
            }
        }
    }

    strcpy(prevdirectory, curdirectory);
}