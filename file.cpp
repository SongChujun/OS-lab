#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<sys/stat.h>
#include<unistd.h>
#include<sys/types.h>
#include<linux/limits.h>
#include<dirent.h>
#include<grp.h>
#include<pwd.h>
#include<errno.h>
#include<malloc.h>
int recursion(int, char *);
int globarcnt=0;
char originalstr[1024];
int main(int argc, char **argv)
{
    int i;
    char path[1024];
    for(i = 1; i < argc; i++)
    {
        if(argv[i][0] != '-')
        {
            strcpy(path, argv[i]);
        }
    }
    struct stat buf;
    lstat(path, &buf);
    if(!S_ISDIR(buf.st_mode))
    {
        printf("%s\n", path);
        return 0;
        //recursion(flag, tmppathname);
    }
    strcpy(originalstr,path);
    recursion(0, path);
    return 0;
}
int recursion(int flag, char *path)
{

    char saveptr[200][300]; 
    DIR *dir;
    struct dirent *ptr;
    char pathname[10000];     //带有文件名称的路径
    int flag_param = 0;
    struct stat buf;
    int len, i, j;
    int cnt=0;
    char display[10000];
    if(globarcnt==0)
    {
        // printf("%s:\n",path);
    }
    else
    {
        int i=0;
        for(i=0;;i++)
        {
            if(path[i]!=originalstr[i])
            {
                break;
            }
        }
//        i++;
        j=0;
        for(;i<strlen(path)+1;i++)
        {
            display[j++]=path[i];
        }
        // display[j]='\0';
        printf("\n.%s:\n",display);
    }
    globarcnt++;
    // display_dir(flag_param, path);
    //pathname = (char *)malloc(PATH_MAX);
    strcpy(pathname, path);
    char tmppathname[10000];
    dir = opendir(path);
    if(dir==NULL)
    {
        return 1;
    }
    for(i=0;i<200;i++)
    {
        for(j=0;j<300;j++)
        {
            saveptr[i][j]='\0';
        }
    }
    cnt=0;
    while(1)
    {
        ptr = readdir(dir);
        if(ptr == NULL)
        {
            for(j=0;j<cnt;j++)
            {
                recursion(flag, saveptr[j]);
            }
            // free(saveptr);
            // printf("\n");
            closedir(dir);
            return 1;
        }
        if(!(strcmp(ptr->d_name,".") && (strcmp(ptr->d_name, "..") && ptr->d_name[0] != '.')))
        {
            continue;
        }
        for(i=0;i<10000;i++)
        {
            tmppathname[i]='\0';
        }
        strcpy(tmppathname,pathname);
        tmppathname[strlen(tmppathname)]='/';
        strcat(tmppathname, ptr->d_name);
        lstat(tmppathname, &buf);
        if(S_ISDIR(buf.st_mode))
        {
            len = strlen(tmppathname);
            pathname[len] = '/';
            pathname[len + 1] = '\0';
            printf("%s\n", ptr->d_name);
            for(j=0;j<strlen(tmppathname);j++)
            {
                saveptr[cnt][j]=tmppathname[j];
            }
            cnt++;
            //recursion(flag, tmppathname);
        }
        else
        {
            printf("%s\n",ptr->d_name);
        }
        // for(i = strlen(path); i < len ; i++)
        // {
        //     pathname[i] = '\0';
        // }
    }
}