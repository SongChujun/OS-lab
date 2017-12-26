#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include<sys/types.h>
#include<sys/wait.h>
pid_t pid1;
pid_t pid2;
void funcmain(int );
void funcchileprocess1(int);
void funcchileprocess2(int);
int main(void)
{
    int n;
    int status;
    pid_t pid_1, pid_2;
    int fd[2];
    char buffer[100] = "main function!";
    signal(SIGINT,funcmain);
    if(pipe(fd) < 0)
    {
        perror("pipe failed!");
        exit(1);
    }
    
    if((pid_1 = fork()) < 0)
    {
        perror("fork 1 failed!");
        exit(1);
    }
    else if(pid_1 == 0)
    {
        // printf("This is the NO.1 child process\n");
        // printf("fucku\n");
        signal(SIGINT,SIG_IGN);
        signal(17,funcchileprocess1);
        char buf[4096];
        int cnt=0;
        // strcpy(buffer, "info from process 1\n");
        //printf("%d\n", strlen(buffer));
        while(1)
        {
            sprintf(buffer,"%d",cnt);
            // printf("%lu\n",strlen(buffer));
            size_t res=write(fd[1], buffer, strlen(buffer));
            // printf("%lu\n",res);
            cnt++;    
            sleep(1);
        }
    }
    if((pid_2 = fork()) < 0)
    {
        perror("fork 2 failed!");
        exit(1);
    }
    else if(pid_2 == 0)
    {
        signal(SIGINT,SIG_IGN);
        signal(18,funcchileprocess2);
        char buf[4096];
        int i=0;
        while(1)
        {
            n = read(fd[0], buffer, 100);
            for( i=0;;i++)
            {
                if(!((buffer[i]<='9')&&(buffer[i]>='0')))
                {
                    break;
                }
            }
            strncpy(buf,buffer,i);
            buf[i+1]='\0';
            // printf("do something!");
            printf("%s\n", buf);
            // printf("%lu\n",strlen(buffer));
        }
    }
    waitpid(pid_1, &status, 0);
    waitpid(pid_2, &status, 0);
    
    // printf("%s\n", buffer);
    
    return 0;
}
void funcmain(int x)
{
    kill(pid1,17);
    kill(pid2,18);
}
void funcchileprocess1(int x)
{
    // close(fd[1]);
    exit(0);
}
void funcchileprocess2(int x)
{
    // close(fd[2]);
    exit(0);
}