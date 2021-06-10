#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<stdlib.h>
#include<string.h>

#include "prompt.h"

extern char prompt_line[4000];

void prompt(char homedirectory[])
{
    strcpy(prompt_line, "<");
    strcpy(username, getlogin());
    strcat(prompt_line, username);
    strcat(prompt_line, "@");
    gethostname(hostname, sizeof(hostname));
    strcat(prompt_line, hostname);
    strcat(prompt_line, ":");
    getcwd(curdirectory, sizeof(curdirectory));

    //~ displayed if new directory is same as Wsh invoking directory

    if(strcmp(curdirectory, homedirectory)==0)
        strcat(prompt_line, "~");
    else
        strcat(prompt_line, curdirectory);
    strcat(prompt_line, "> ");

}