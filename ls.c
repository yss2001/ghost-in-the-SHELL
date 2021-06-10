#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<string.h>
#include<dirent.h>
#include<pwd.h>
#include<grp.h>
#include<time.h>

#include "ls.h"

extern char curdirectory[4000];
extern char home_directory[4000];

DIR *directory;
struct dirent *dir;

void ls_print_utility(int flag_a)
{
    //miscellaenous variables
    int max_link_size=0, max_user_size=0, max_group_size=0, max_size_size=0;
    struct stat file;
    struct passwd *names1;
    struct group *names2;
    struct stat timebuff;
    char time_string[100];
    char int_to_string[20];
    
    //found max sizes of each column
    int record_count = 0;
    while((dir = readdir(directory)) != NULL)
    {
        //some files ignored if flag_a = 0
        if(!flag_a && (dir->d_name[0]=='.' || !strcmp(dir->d_name, "..")))
            continue;
        if(lstat(dir->d_name, &file)<0)
            perror("Error");

        //digits in hard link found
        sprintf(int_to_string, "%lu", file.st_nlink);
        field_sizes[record_count][0] = strlen(int_to_string);
        if(field_sizes[record_count][0]>max_link_size)
            max_link_size = field_sizes[record_count][0];

        //length of username found
        names1 = getpwuid(file.st_uid);
        field_sizes[record_count][1] = strlen(names1->pw_name);
        if(field_sizes[record_count][1]>max_user_size)
            max_user_size = field_sizes[record_count][1];


        //length of groupname found
        names2 = getgrgid(file.st_gid);
        field_sizes[record_count][2] = strlen(names2->gr_name);
        if(field_sizes[record_count][2]>max_group_size)
            max_group_size = field_sizes[record_count][2];

        //length of size found
        sprintf(int_to_string, "%ld", file.st_size);
        field_sizes[record_count][3] = strlen(int_to_string);
        if(field_sizes[record_count][3]>max_size_size)
            max_size_size = field_sizes[record_count][3];

        record_count+=1;
    }

    seekdir(directory, 0);

    //traversed again to print
    record_count=0;
    while((dir = readdir(directory)) != NULL)
    {
        //some files ignored if flag_a = 0
        if(!flag_a && (dir->d_name[0]=='.' || !strcmp(dir->d_name, "..")))
            continue;
        if(lstat(dir->d_name, &file)<0)
            perror("Error");
        
        //file type checked
        if(S_ISDIR(file.st_mode))
            printf("d");
        else if(S_ISREG(file.st_mode))
            printf("-");
        else if(S_ISLNK(file.st_mode))
            printf("l");
        else if(S_ISBLK(file.st_mode))
            printf("b");
        else if(S_ISCHR(file.st_mode))
            printf("c");
        else if(S_ISFIFO(file.st_mode))
            printf("p");
        else if(S_ISSOCK(file.st_mode))
            printf("s");

        //file permissions processed
        if(file.st_mode & S_IRUSR)
            printf("r");
        else 
            printf("-");
        
        if(file.st_mode & S_IWUSR)
            printf("w");
        else 
            printf("-");

        if(file.st_mode & S_IXUSR)
            printf("x");
        else 
            printf("-");
        
        if(file.st_mode & S_IRGRP)
            printf("r");
        else 
            printf("-");
        
        if(file.st_mode & S_IWGRP)
            printf("w");
        else 
            printf("-");

        if(file.st_mode & S_IXGRP)
            printf("x");
        else 
            printf("-");

        if(file.st_mode & S_IROTH)
            printf("r");
        else 
            printf("-");
        
        if(file.st_mode & S_IWOTH)
            printf("w");
        else 
            printf("-");

        if(file.st_mode & S_IXOTH)
            printf("x ");
        else 
            printf("- ");
        
        //number of hard links printed
        sprintf(int_to_string, "%lu", file.st_nlink);
        printf("%*s ", max_link_size, int_to_string);

        //owner username printed
        names1 = getpwuid(file.st_uid);
        printf("%*s ", -1*max_user_size, names1->pw_name);

        //group name printed
        names2 = getgrgid(file.st_gid);
        printf("%*s ", -1*max_group_size, names2->gr_name);

        //file size printed
        printf("%*ld ", max_size_size, file.st_size);        

        //last modification time printed
        stat(dir->d_name, &timebuff);
        strcpy(time_string, ctime(&timebuff.st_mtime));
        time_string[strlen(time_string)-1] = '\0';
        printf("%.12s ", time_string+4);

        //entry name printed
        printf("%s\n", dir->d_name);
    }

}
void ls_print(char dir_name[], int flag_a, int flag_l)
{
    //correct directory opened
    if(strcmp(dir_name, "cur")==0)
        directory = opendir(curdirectory);
    else 
    {
        if(!strcmp(dir_name, "~"))
            directory = opendir(home_directory);
        
        else if((directory = opendir(dir_name))==NULL)
        {
            perror("Error");
            return;
        }
    }

    if(strcmp("cur", dir_name))
    {
        chdir(dir_name);
    }

    while((dir = readdir(directory)) != NULL)
    {
        if(!flag_l & !flag_a)
        {
            if(dir->d_name[0]!='.' && strcmp(dir->d_name, ".."))
                printf("%s\n", dir->d_name);
        }
        else if(flag_a && !flag_l)
            printf("%s\n", dir->d_name);
        //control sent to utility if flag_l is true
        else 
            ls_print_utility(flag_a);
    }

    closedir(directory);
}

void ls(char command[])
{
    chdir(curdirectory);
    int flag_l=0, flag_a=0;
    //if command is just ls
    if(command[2]=='\0')
        ls_print("cur", flag_a, flag_l);
    else
    {
        //all input flags noted
        int i=3, j=0;
        while(command[i]!='\0' && i<strlen(command))
        {
            j=0;
            while(command[i]!=' ' && i<strlen(command))
            {
                ls_segment[j] = command[i];
                i++, j++;
            }
            ls_segment[j] = '\0';
            
            if(!strcmp(">", ls_segment) || !strcmp(">>", ls_segment) || !strcmp("<<", ls_segment))
                break;
            if(!strcmp("-l", ls_segment))
                flag_l = 1;
            else if(!strcmp("-a", ls_segment))
                flag_a = 1;
            else if(!strcmp("-al", ls_segment) || !(strcmp("-la", ls_segment)))
                flag_a = flag_l = 1;
            
            if((flag_a==0 || flag_l==0) && (i<strlen(command)))
                while(command[i]==' ')
                    i++;
            if(flag_l == 1 && flag_a == 1)
                break;
        }
        //input directories processed
        i=3, j=0;
        int called =0;
        while(command[i]!='\0')
        {
            j=0;
            while(command[i]!=' ' && i<strlen(command))
            {
                ls_segment[j] = command[i];
                i++, j++;
            }

            if(i<strlen(command))
            {
                while(command[i]==' ')
                    i++;
            }
            ls_segment[j] = '\0';
            
            if(!strcmp(">", ls_segment) || !strcmp(">>", ls_segment) || !strcmp("<", ls_segment))
            {
                break;
            }
            if(strcmp("-l", ls_segment) && strcmp("-a", ls_segment) && strcmp("-al", ls_segment) && strcmp("-la", ls_segment))
            {
                printf("%s:\n", ls_segment);
                ls_print(ls_segment, flag_a, flag_l); 
                called=1;
                chdir(curdirectory);
            }
        }
        //if only flags were specified
        if(!called)
            ls_print("cur", flag_a, flag_l);
        
    }

    chdir(curdirectory);
}