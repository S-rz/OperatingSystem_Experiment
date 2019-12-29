#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

#define BLOCK_SIZES 5 //����������
#define BUFFER_SIZE 4096  //��������С

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};
union semun arg;
int semid;//�ź��������׵�ַ
struct share_buffer
{
    int end;
    char buffer[BUFFER_SIZE];
    struct share_buffer *next;
};
struct share_buffer *start=NULL;

char *share[BLOCK_SIZES];//ָ��������
int pid1,pid2;//�ӽ��̱�ʶ��
int running=1;
int shmid[BLOCK_SIZES];//����������ʶ��

void P(int semid, int index)��
void V(int semid, int index)��
void writebuf()��
void readbuf();

int main()
{
  	int ret;
  	int i;
  	void* shm=NULL;
  	key_t key=1234;
  	struct share_buffer *head=NULL,*tail=NULL;
  	int size=sizeof(struct share_buffer);
  	/***���������ڴ���***/
  	for(i=0;i<BLOCK_SIZES;i++)
    {
    		shmid[i]=shmget(key,size,0666|IPC_CREAT);
    		if (shmid[i]==-1)
        {
    		    fprintf(stderr, "shmget fail\n");
    		    exit(EXIT_FAILURE);
    		}
    		key++;
    		//�������ڴ����ӵ���ǰ���̵ĵ�ַ�ռ�
    		shm=shmat(shmid[i],(void*)0,0);
    		if(shm==(void*)-1)fprintf(stderr, "shmat fail\n");
    		//���ù����ڴ�
    		head=(struct share_buffer *)shm;
    		head->end=0;
    		head->next=tail;
    		if(i==0)start=head;
    		tail=head;
  	}
  	start->next=head;
    /***���������ź���***/
    semid=semget(key,2,IPC_CREAT|0666);
    if(semid==-1)
    {
         perror("create semget error");
         return ;
    }
    /***�������ź�������ֵ***/
  	arg.val=BLOCK_SIZES;
    ret=semctl(semid,0,SETVAL,arg);//�ź���semid��0��Ϊ1
  	arg.val=0;
  	ret =semctl(semid,1,SETVAL,arg);//�ź���semid��1��Ϊ0
    if (ret < 0 ){
        perror("ctl sem error");
        semctl(semid,0,IPC_RMID,arg);
        return -1 ;
    }
  	/***�����ӽ���***/
    if((pid1=fork())<0)
    {
          perror("Fail to fork");
          exit(EXIT_FAILURE);
    }
  	else if(pid1==0)writebuf();
   	else {
  		if((pid2=fork())<0){
  			perror("Fail to fork");
  			exit(EXIT_FAILURE);
  		}
  		else if(pid2==0)readbuf();
  		else
  		{
    			/***�ȴ��ӽ��̽���***/
    			waitpid(pid1,NULL,0);
    			waitpid(pid2,NULL,0);
    			printf("Child Fanished!\n");
    			/***ɾ���ź���***/
    			semctl(semid,0,IPC_RMID,arg);
    			semctl(semid,1,IPC_RMID,arg);
    			/***ɾ��������***/
    			for(i=0;i<BLOCK_SIZES;i++)
      				if (shmctl(shmid[i],IPC_RMID,0)==-1)
              {
      				    fprintf(stderr, "shmctl(IPC_RMID) failed\n");
      				    exit(EXIT_FAILURE);
    				  }
  			  printf("Father Fanished!\n");
  		}
  	}
  	return 0;
}

void writebuf()
{
    int file_end;
  	int times=0;
  	const char* pathname;
  	struct share_buffer *in=start;
  	int fd;//�ļ�������
  	char pn[100]="./Harry.rmvb";
  	pathname=pn;
  	if ((fd=open(pathname,O_RDONLY))==-1){
  		printf("File Open Error��\n");
  		return;
  	}
  	else
  		printf("File Open Success!\n");
  	while(running)
    {
    		P(semid,0);
    		file_end=read(fd,in->buffer, BUFFER_SIZE-1);
    		if(file_end!=BUFFER_SIZE-1&&file_end!=0)
        {
      			printf("Reading %d Bytes in the last time!\n",file_end);
      			in->end=file_end;
      			V(semid,1);
      			return;
    		}
    		printf("Reaing File in the %dth time!\n",++times);
    		in=in->next;
    		V(semid,1);
  	}
  	exit(EXIT_SUCCESS);
}

void readbuf()
{
  	int times=0;
  	const char* pathname;
  	struct share_buffer *out=start;
  	int fd;//�ļ�������
  	char pn[100]="./poter.rmvb";
  	pathname=pn;
  	if ((fd=open(pathname,O_WRONLY|O_CREAT,S_IRWXU|S_IXGRP|S_IROTH|S_IXOTH))==-1)
    {
    		printf("File Open Error��\n");
    		return;
  	}
  	else
  		printf("File Open Success!\n");
  	while(running)
    {
    		P(semid,1);
    		write(fd,out->buffer, BUFFER_SIZE-1);
    		printf("Writing File in the %dth time!\n",++times);
    		out=out->next;
    		V(semid,0);
    		if(out->end!=0)//д���ļ�β
    		{
      			printf("Writing %d Bytes in the last time!\n",out->end);
      			write(fd,out->buffer,out->end);
      		 	return ;
    		}
  	}
  	exit(EXIT_SUCCESS);
}

/***���ź�������index��ŵ��ź�����P����***/
void P(int semid, int index){
    struct sembuf sem={index,-1,0};
    semop(semid,&sem,1);
}

/***���ź�������index��ŵ��ź�����V����***/
void V(int semid, int index){
    struct sembuf sem={index,+1,0};
    semop(semid,&sem,1);
}
