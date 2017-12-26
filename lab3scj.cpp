#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <semaphore.h>
#include <string.h>
#include <sys/stat.h>
#define PACKET_SIZE 1024//inbytes
#define MAX_SIZE 8
#define SHM_KEY_T 9494
#define SEM_KEY_T 8989
sem_t writesem;
sem_t readsem;
void readbufProcess(char *);
void writebufProcess(char *);
int fileLength=0;
int  readbuf;//用于存储子进程的pid
int  writebuf;
char* shmptr[MAX_SIZE];
union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO(Linux-specific) */
};
void P(int semid){
    struct sembuf op;
    op.sem_num = 0;
    op.sem_op = -1;
    op.sem_flg = 0;
    semop(semid, &op, 1);
}
void V(int semid){
    struct sembuf op;
    op.sem_num = 0;
    op.sem_op = 1;
    op.sem_flg = 0;
    semop(semid, &op, 1);
}

int semReadId, semWriteId;//semaphore
int mutex;
int main(int argc,char **argv )
{
    if(argc<3)
    {
        printf("two few arguments\n");
        return 0;
    }
    sem_init(&writesem,1,0);
    sem_init(&readsem,1,1);
    struct stat st;
    stat(argv[1], &st);
    fileLength = st.st_size;
    // printf("%d\n",fileLength);
    int i, shmid[MAX_SIZE];
    for(i = 0; i < MAX_SIZE; ++i){
        shmid[i] = shmget(IPC_PRIVATE, PACKET_SIZE, IPC_CREAT | 0666);//init
        if(shmid[i] == -1){
            printf("%d is the max number of shm in sys", i);
            break;
        }
    }
    int j;
    for(j = 0; j < i; ++j){
        shmptr[j] = (char*)shmat(shmid[j], NULL, SHM_R | SHM_W);//connect
    }
    semReadId = semget(IPC_PRIVATE, 1, IPC_CREAT);
    semWriteId =semget(IPC_PRIVATE, 1, IPC_CREAT);
    mutex =semget(IPC_PRIVATE, 1, IPC_CREAT);
    if(semReadId == -1 || semWriteId == -1||mutex==-1)
    {
        printf("sem get error\n");
        exit(1);
    }
    union semun arg1, arg2,arg3;
    arg1.val = MAX_SIZE;
    if(semctl(semReadId, 0, SETVAL, arg1) == -1)
    {
        printf("sem set fail\n");
        exit(1);
    }
    arg2.val = 0;
    if(semctl(semWriteId, 0, SETVAL, arg2) == -1)
    {
        printf("sem set fail\n");
        exit(1);
    }
    arg3.val=1;
    if(semctl(mutex, 0, SETVAL, arg3) == -1)
    {
        printf("sem set fail\n");
        exit(1);
    }
    readbuf=fork();
    if(readbuf==0)
    {
        readbufProcess(argv[1]);
        return 0;
    }
    else
    {
        writebuf=fork();
        if(writebuf < 0)
        {
            perror("fork 2 failed!");
            exit(1);
        }
        else if(writebuf==0)
        {
            writebufProcess(argv[2]);
            return 0;
        }
        waitpid(readbuf, NULL, 0);
        waitpid(writebuf, NULL, 0);
        return 0;
    }
}
void readbufProcess(char * readfilename){
    // printf("readbufprocess\n");
    FILE *fp;
    fp=fopen(readfilename,"rb");
    if(fp==NULL)
    {
        printf("open source file  failed!\n");
        return;
    }
    int readnum=0;
    int index=0;
    // printf("readbufwhile\n");
    fflush(stdout);
    while(1)
    {
        // printf("readbufbef\n");
        P(semReadId);
        // printf("readbufaft\n");
        P(mutex);
        readnum=fread(shmptr[index], sizeof(char),PACKET_SIZE,fp);
        // printf("%d\n",readnum);
        if(readnum<PACKET_SIZE)
        {
            V(mutex);
            V(semWriteId);
            break;
        }
        index=(index+1)%MAX_SIZE;
        V(mutex);
        V(semWriteId);
    }
    fclose(fp);
    return ;
}

void writebufProcess(char* writefilename){
    // printf("writebufprocess\n");
    FILE *fp;
    fp=fopen(writefilename,"wb");
    if(fp==NULL)
    {
        printf("open target file failed!\n");
        return ;
    }
    int writtennum=0;
    int totalbytes=0;
    int index=0;
    // printf("writebufwhile\n");
    fflush(stdout);
    while(totalbytes<fileLength)
    {
        // printf("writebufbef\n");
        P(semWriteId);
        // printf("writebufaft\n");
        P(mutex);
        int n=totalbytes+PACKET_SIZE-fileLength;
        // printf("%d\n",n);
        if(totalbytes+PACKET_SIZE>=fileLength)
        {
            writtennum=writtennum=fwrite(shmptr[index],sizeof(char),fileLength-totalbytes,fp);
            printf("Mission Complete\n");
        }
        else
        {
            writtennum=fwrite(shmptr[index],sizeof(char),PACKET_SIZE,fp); 
        }
        if(writtennum==-1)
        {
            printf("write error\n");
            break;
        }
        index=(index+1)%MAX_SIZE;
        totalbytes+=writtennum;
        V(mutex);
        V(semReadId);
    }
    fclose(fp);
    return ;
}