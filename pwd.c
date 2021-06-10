#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<stdlib.h>
#include<string.h>

#include "pwd.h"

void pwd_print()
{
    getcwd(pwd, sizeof(pwd));
    printf("%s\n", pwd);
}