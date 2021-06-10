#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<stdlib.h>
#include<string.h>
#include<sys/wait.h>
#include<sys/file.h>

#include "history.h"

extern char home_directory[4000];
extern char command[4000];
void store_history(int total)
{
    int tempfile, offset=0;
    char file_path[4000], temp_path[4000];

    //paths made in case history is issued outside home
    strcpy(file_path, home_directory);
    strcpy(temp_path, home_directory);

    strcat(file_path, "/history.txt");
    strcat(temp_path, "/temp.txt");
    int outfile = open(file_path, O_RDWR);
    lseek(outfile, 0, 2);
    write(outfile, command, strlen(command));
    write(outfile, "\n", 1);

    //replacement of history more than 20
    if(total+1>20)
    {
        char c;
        int newline=0;
        tempfile = open(temp_path, O_WRONLY | O_CREAT, 0666);
        lseek(outfile, 0, 0);

        while(read(outfile, &c, 1))
        {
            if(c=='\n')
                break;
        }
        while(read(outfile, &c, 1))
        {
            write(tempfile, &c, 1);
        }

        remove(file_path);
        chdir(home_directory);
        rename("temp.txt", "history.txt");

    }
}

//history loaded and total returned
int get_history()
{
    int total=0;
    int inpfile = open("history.txt", O_RDONLY);

    char c;
    int index=0, j=0;

    while(read(inpfile, &c, 1))
    {
        if(c=='\n')
        {
            total++;
        }
    }

    close(inpfile);
    return total;
}

void print_history(char command[])
{
    int num=0;
    char part[300];

    int i=7;
    while(i<strlen(command))
    {
        int j=0;
        while(command[i]!=' ' && i<strlen(command))
        {
            part[j] = command[i];
            i++, j++;
        }

        part[j] = '\0';

        if(!strcmp(part, ">") || !strcmp(part, "<") || !strcmp(part, ">>"))
            break;
        else 
            num = atoi(part);
        
        while(i<strlen(command) && command[i]==' ')
            i++;
    }

    if(num==0)
        num=10;
    
    if(num<0 || num>20)
        num = 10;
   
    char file_path[4000];
    strcpy(file_path, home_directory);
    strcat(file_path, "/history.txt");

    int file = open(file_path, O_RDONLY);
    char c;
    int newlines = 0;
    int total = 0;

    total = get_history();

    //required number of entries printed
    if(total<=num)
    {
        while(read(file, &c, 1))
        {
            if(c=='\n')
                newlines+=1;

            printf("%c", c);
            if(newlines==total)
                break;
        }
    }
    else
    {
        while(read(file, &c, 1))
        {
            if(c=='\n')
                newlines+=1;
        
            if(newlines==(total-num))
                break;
        }

        while(read(file, &c, 1))
            printf("%c", c);
    }
}