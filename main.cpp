#include <iostream>
#include <pthread.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <semaphore.h>
#define SEM_KEY 6666
void P(int semid,int index);
void v(int semid,int index);
int init_sem(int sem_id,int init_value);
int del_sem(int sem_id);
void* print_fn(void* arg);
void* cal_fn(void* arg);
int a;
int sem_id;
sem_t sema;
sem_t semb;
void P(int semid,int index)
{
//    struct sembuf sem;
//    sem.sem_num=index;
//    sem.sem_op=-1;
//    sem.sem_flg=0;
//    semop(semid,&sem,1);
//    return;
}
void V(int semid,int index)
{
//    struct sembuf sem;
//    sem.sem_num=index;
//    sem.sem_op=1;
//    sem.sem_flg=0;
//    semop(semid,&sem,1);
//    return;
}
int main()
{
    pthread_t pcal;
    pthread_t pprint;
//    int err;
//    err=pthread_create(&pcal,NULL,cal_fn,NULL);
//    int sem_id;
//    sem_id=semget(SEM_KEY,1,IPC_CREAT | 0666);
    sem_init(&sema,0,0);
    sem_init(&semb,0,1);
    pthread_create(&pcal,NULL,cal_fn,NULL);
    pthread_create(&pprint,NULL,print_fn,NULL);
    pthread_join(pcal,NULL);
    pthread_join(pprint,NULL);
    sem_destroy(&sema);
    sem_destroy(&semb);
    return 0;
}
int init_sem(int sem_id,int init_value)
{
//    union semun sem_union;
//    sem_union.val=init_value;
//    if(semctl(sem_id,0,SETVAL,sem_union)==-1)
//    {
//        perror("Initialize semaphore");
//        return -1;
//    }
//    return 0;
}
int del_sem(int sem_id)
{
//    if(semctl(sem_id,0,IPC_RMID)==-1)
//    {
//        perror("Delete semaphore");
//        return -1;
//    }
}
void* cal_fn(void* arg)
{
    int i=0;
    for(i=0;i<100;i++)
    {
        sem_wait(&semb);
        a=a+1;
        sem_post(&sema);
    }
    return ((void*)0);

}
void* print_fn(void* arg)
{
    int i=0;
    int sum=0;
    for(i=0;i<100;i++)
    {
        sem_wait(&sema);
        sum+=a;
        printf("%d\n",sum);
        sem_post(&semb);
    }
    ((void*)0);
}
