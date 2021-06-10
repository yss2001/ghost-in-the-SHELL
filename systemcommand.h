#ifndef SYSTEMCOMMAND_H
#define SYSTEMCOMMAND_H

int pid;
char *arguments[35000];

struct process_names{
    int process_id;
    char process_name[256];
}

process_names[256];

void print_termination(int x);
void system_command(char command[]);


#endif