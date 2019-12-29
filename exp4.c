#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sem.h>

void display(unsigned short mode);
void printdir(char *dir,int depth);

int main(){
    int depth=0;
    char dir[500];
    printf("Please Input The Directory:\n");
    scanf("%s",dir);
    printdir(dir,depth);
    return 0;
}

void printdir(char *dir,int depth)
{
    int flag=0;
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    if(!(dp=opendir(dir)))
    {
        printf("Fail To Open The Directory!\n");
        return;
    }
    chdir(dir);
    while((entry=readdir(dp))!=NULL)
    {
        lstat(entry->d_name,&statbuf);
        if(S_ISDIR(statbuf.st_mode))
        {
            if(strcmp(entry->d_name,".")==0||strcmp(entry->d_name,"..")==0)
            {
                continue;
            }
            display(statbuf.st_mode);
            printf("%d\t",depth);
            printf("%d\t%d\t",statbuf.st_uid,statbuf.st_gid);
            printf("%ld\t",statbuf.st_size);
            printf("%s\t",strtok(ctime(&statbuf.st_mtime)+4,"\n"));
            printf("%s\n",entry->d_name);
            printdir(entry->d_name,depth+4);
        }
        else
        {
            display(statbuf.st_mode);
            printf("%d\t",depth);
            printf("%d\t%d\t",statbuf.st_uid,statbuf.st_gid);
            printf("%ld\t",statbuf.st_size);
            printf("%s\t",strtok(ctime(&statbuf.st_mtime)+4,"\n"));
            printf("%s\n",entry->d_name);
        }
    }
    chdir("..");
    closedir(dp);//关闭目录流
}

void display(unsigned short mode)
{
    if(mode&S_IFDIR)
        printf("d");
    else
        printf("-");
    if(mode&S_IRUSR)
        printf("r");
    else
        printf("-");
    if(mode&S_IWUSR)
        printf("w");
    else
        printf("-");
    if(mode&S_IXUSR)
        printf("x");
    else
        printf("-");
    if(mode&S_IRUSR)
        printf("r");
    else
        printf("-");

    if(mode&S_IRGRP)
        printf("r");
    else
        printf("-");
    if(mode&S_IWGRP)
        printf("w");
    else
        printf("-");
    if(mode&S_IXGRP)
        printf("x");
    else
        printf("-");

    if(mode&S_IROTH)
        printf("r");
    else
        printf("-");
    if(mode&S_IWOTH)
        printf("w");
    else
        printf("-");
    if(mode&S_IXOTH)
        printf("x");
    else
        printf("-");

    printf("\t");
}
