#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<stdlib.h>
#include<string.h>
#include<sys/wait.h>
#include<sys/file.h>
#include<termios.h>
#include<fcntl.h>

#include "nightswatch.h"

int proc_file;
int offset=0, interval;

void interrupt_printer(int interval)
{
    //termios set up to make input non canonical
    //fcntl set up to make input non blocking
    struct termios old, new;
    tcgetattr(0, &old);
    tcgetattr(0, &new);
    new.c_lflag = new.c_lflag & (~ECHO);
    new.c_lflag = new.c_lflag & (~ICANON);
    tcsetattr(0, TCSANOW, &new);

    int fcntl_val = fcntl(0, F_SETFL, O_NONBLOCK);
    char quit;
    int c_read;
    char inp;

    while(1)
    {
        //read and printed IRQ 1 until q is pressed
        for(int q = 1; q<=interval; q++)
        {
            quit = getchar();
            getchar();
            if(quit=='q' || quit=='Q')
                break;
            sleep(1);
        }

        if(quit=='q' || quit=='Q')
        {
            break;
        }
        
        //sleep(interval);
        lseek(proc_file, offset, 0);
        c_read=0;
        while(read(proc_file, &inp, 1))
        {
            if(inp=='\n')
                break;
        }

        while(read(proc_file, &inp, 1))
        {
            c_read += 1;
            if(c_read==5)
                break;
        }


        while(read(proc_file, &inp, 1))
        {
            if(inp=='I')
                break;
            else 
            {
                printf("%c", inp);
            }
        }
        printf("\n");
    }

    //reset original input configuration
    tcsetattr(0, TCSANOW, &old);
    fcntl_val &= ~O_NONBLOCK;
    fcntl(0, F_SETFL, fcntl_val);


}
void watch_printer(char command[])
{
    interval = 2;
    int i = 12, j=0, c=-1, nflag=0;
    //c = 0 if interrupt, c = 1 if newborn, c = -1 if user didn't specify
    while(command[i]!='\0')
    {
        //command parsed for arguments
        j = 0;
        while(command[i]!=' ' && i<strlen(command))
        {
            watch_segment[j] = command[i];
            i++,j++;
        }
        watch_segment[j] = '\0';

        if(!strcmp(watch_segment, ">") || !strcmp(watch_segment, ">>") || !strcmp(watch_segment, "<"))
            break;
        if(!strcmp("interrupt", watch_segment))
            c = 0;
        else if(!strcmp("newborn", watch_segment))
            c = 1;
        else if(!strcmp("-n", watch_segment))   
            nflag = 1;
        else
        {
            int flag=0;
            for(int k=j-1; k>=0; k--)
            {
                if(!(watch_segment[k]>=48 && watch_segment[k]<=57))
                {
                    flag=1;
                    break;
                }
            }

            //converted time interval to integer
            int x;
            if(!flag)
            {
                x = atoi(watch_segment);
                interval = x;
            }
            if(interval==0 || nflag==0)
                interval = 2;

        }

        if(i<strlen(command))
                while(command[i]==' ')
                    i++;
    }
    
    //branch for interrupt
    if(c==0)
    {
        offset=0;
        proc_file = open("/proc/interrupts", O_RDONLY);

        int c_read=0;
        char inp;

        //setting position to read IRQ1
        while(read(proc_file, &inp, 1))
        {
            c_read += 1;
            offset += 1;
            if(c_read==5)
                break;
        }

        while(read(proc_file, &inp, 1))
        {
            offset += 1;
            printf("%c", inp);
            if(inp=='\n')
                break;
        }

        interrupt_printer(interval);
        close(proc_file);
    }
    else 
    {
        printf("Not supported!\n");
        return;
    }

    int val = fcntl(0, F_GETFL, 0);
    val &= ~O_NONBLOCK;
    fcntl(0, F_SETFL, val);
}
