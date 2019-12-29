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

int ticket_num=100;
int semid;
semun arg;
pthread_t p1,p2,p3;

void *subp1(void *);
void *subp2(void *);
void *subp3(void *);
void P(int semid, int index);
void V(int semid,int index);

int main(){
    //信号灯
    semid=semget(IPC_PRIVATE,1,IPC_CREAT|0666);
    arg.val=1;
    semctl(semid,0,SETVAL,arg);
    //线程
    pthread_create(&p1,NULL,subp1,NULL);
    pthread_create(&p2,NULL,subp2,NULL);
    pthread_create(&p3,NULL,subp3,NULL);
    //等待线程结束
    pthread_join(p1,NULL);
    pthread_join(p2,NULL);
    pthread_join(p3,NULL);
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
        if(ticket_num>0)
        {
            cout<<"This thread is p1!"<<endl;
            ticket_num--;
            cout<<"Now left ticket is "<<ticket_num<<endl<<endl;
        }
        V(semid,0);
        if(ticket_num<=0)
        {
            pthread_exit(NULL);
        }
    }
}

void *subp2(void *){
  for(;1;)
  {
      P(semid,0);
      if(ticket_num>0)
      {
          cout<<"This thread is p2!"<<endl;
          ticket_num--;
          cout<<"Now left ticket is "<<ticket_num<<endl<<endl;
      }
      V(semid,0);
      if(ticket_num<=0)
      {
          pthread_exit(NULL);
      }
  }
}

void *subp3(void *){
  for(;1;)
  {
      P(semid,0);
      if(ticket_num>0)
      {
          cout<<"This thread is p3!"<<endl;
          ticket_num--;
          cout<<"Now left ticket is "<<ticket_num<<endl<<endl;
      }
      V(semid,0);
      if(ticket_num<=0)
      {
          pthread_exit(NULL);
      }
  }
}
