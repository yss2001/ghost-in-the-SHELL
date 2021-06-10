#ifndef LS_H
#define LS_H

int permissions[9];
int field_sizes[2000][4];
//field_sizes is for displaying columns in organized manner
//[][0] for hard links
//[][1] for username
//[][2] for group name
//[][3] for size
char ls_segment[4000];

void ls_print_utility(int flag_a);
void ls_print(char dir_name[], int flag_a, int flag_l);
void ls(char command[]);

#endif