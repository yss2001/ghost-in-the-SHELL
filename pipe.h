#ifndef PIPE_H
#define PIPE_H

char *args[35000];
char part[400], part2[400];

int check_pipe(char command[]);

void parse_pipe(char command[], int num_of_pipes);

void execute_pipe(char **command, int pipe_part);

#endif
