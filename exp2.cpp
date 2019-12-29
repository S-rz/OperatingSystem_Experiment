#include <pthread.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <iostream>

using namespace std;

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *_buf;
};

int a=0;
int num=1;
int semid;
semun arg;
pthread_t p1,p2;

void *subp1(void *);
void *subp2(void *);
void P(int semid, int index);
void V(int semid,int index);

int main(){
    //信号灯
    semid=semget(IPC_PRIVATE,2,IPC_CREAT|0666);
    arg.val=1;
    semctl(semid,0,SETVAL,arg);
    arg.val=0;
    semctl(semid,1,SETVAL,arg);
    //线程
    pthread_create(&p1,NULL,subp1,NULL);
    pthread_create(&p2,NULL,subp2,NULL);
    //等待线程结束
    pthread_join(p1,NULL);
    pthread_join(p2,NULL);
    //删除信号灯
    arg.val=1;
    semctl(semid,1,IPC_RMID,arg);
    return 0;
}

void P(int semid, int index)
{
	struct sembuf sem;
	sem.sem_num = index;
	sem.sem_op = -1;
	sem.sem_flg = 0;
	semop(semid,&sem,1);
	return;
}

void V(int semid,int index)
{
	struct sembuf sem;
	sem.sem_num = index;
	sem.sem_op =  1;
	sem.sem_flg = 0;
	semop(semid,&sem,1);
	return;
}

void *subp1(void *){
    for(;1;)
    {
        P(semid,0);
        cout<<"a="<<a<<endl;
        V(semid,1);
        if(num>=101)
        {
            pthread_exit(NULL);
        }
    }
}

void *subp2(void *){
    for(;1;)
    {
        P(semid,1);
        a=a+num;
	      num++;
        V(semid,0);
        if(num>=101)
        {
            pthread_exit(NULL);
        }
    }
}
