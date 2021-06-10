#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<stdlib.h>
#include<string.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<signal.h>

#include "prompt.h"
#include "commandhandler.h"
#include "history.h"

char prompt_line[4000];

char home_directory[4000];

char command[100000], prev[100000];
int main()
{
    //reset terminal to blocking for safety
    int val = fcntl(0, F_GETFL, 0);
    val &= ~O_NONBLOCK;
    fcntl(0, F_SETFL, val);

    signal(SIGINT, SIG_IGN);

    const char c_delim[] = ";";
    getcwd(home_directory, sizeof(home_directory));

    int history_number = -1;

    history_number = get_history();
    
    while(1)
    {
        //color changed and printed prompt
        printf("\033[1;36m");
        prompt(home_directory);
        printf("%s", prompt_line);
        printf("\033[0m");

        //command is input and null terminated before \n
        if(fgets(command, sizeof(command), stdin) == NULL)
        {
            printf("\n");
            exit(0);
        }
        command[strlen(command)-1] = '\0';

        
        //stored in history
        if(strlen(command)>0 && strcmp(prev, command))
        {                
            store_history(history_number);
            history_number+=1;
        }

        char *command_part = strtok(command, c_delim);

        //tokenized by ';' and sent to commandhandler
        while(command_part!=NULL)
        {
            commandhandler(command_part);
            command_part = strtok(NULL, c_delim);

        }
        
        //stored as previous command for history
        strcpy(prev, command);
    }
    return 0;
}