//线程控制的相关函数
//1.创建线程
//2.终止线程
//3.等待线程
//4.线程分离
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
# if 0
//int arr[1000000]={0};
void * ThreadEntry(void *arg)
{
  (void)arg;
//  size_t i=0;
//  for(i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
//  {
//    arr[i]=i;
//  }
  while(1)
  {
    printf("ThreadEntry %lu\n",pthread_self());
    sleep(1);
  }
    return NULL;
}

int main()
{
  pthread_t tid=0;
  pthread_create(&tid,NULL,ThreadEntry,NULL);
  //while(1)
  //{
  printf("MainThread %lu\n",pthread_self());
  //sleep(1);
  pthread_cancel(tid);
  //}
  pthread_join(tid,NULL);
  printf("已回收线程\n");
  return 0;
}
#endif

//1.线程之间共享内存地址
#if 0
//int g_count=0;  //全局变量地址
void* ThreadEntry(void *arg)
{
  //(void) arg;
  int *p=(int*)arg;
  while(1)
  {
    printf("In ThreadEntry\n");
   // ++g_count;
   ++*p;
    sleep(1);
  }
  return NULL;
}

int main()
{
  //int count=0;   //栈上数据可以访问
  int *p=(int*)malloc(4);   //堆上数据可以访问
  *p=0;
  pthread_t tid;
  //pthread_create(&tid,NULL,ThreadEntry,NULL);
  pthread_create(&tid,NULL,ThreadEntry,p);
  pthread_detach(tid);
  while(1){
    printf("In MainThread %d\n",*p);
    sleep(1);
  }
  return 0;
}
#endif

#if 0
//2.线程异常终止
void* ThreadEntry(void *arg)
{
  (void)arg;
  while(1)
  {
    sleep(1);
    int *p=NULL;
    *p=10;
  }
  return NULL;
}
int main()
{
  pthread_t tid;
  pthread_create(&tid,NULL,ThreadEntry,NULL);
  while(1)
  {
    printf("In MainThread\n");
    sleep(1);
  }
  return 0;

}
#endif

//3.多线程利用多核资源
#if 0
void* ThreadEntry(void *arg)
{
  (void)arg;
  while(1);
  return NULL;
}

int main()
{
  pthread_t tid1,tid2,tid3,tid4;

  pthread_create(&tid1,NULL,ThreadEntry,NULL);
  pthread_create(&tid2,NULL,ThreadEntry,NULL);
  pthread_create(&tid3,NULL,ThreadEntry,NULL);
  pthread_create(&tid4,NULL,ThreadEntry,NULL);
  while(1);
  return 0;
}
#endif

//4.通过多线程提高程序执行效率
//   假设存在一个很大的数组，把数组中的每一个元素都执行一个成方运算
//   再赋值回数组中
#if 0
//单线程版本
#define SIZE 100000000 
int64_t GetUs()     //获取毫秒 
{ 
  struct timeval tv;
  gettimeofday(&tv,NULL);
  return tv.tv_sec *1000000+tv.tv_usec;
} 
void Calc(int*arr,int beg,int end)
{ 
  int i=beg;
  for(i;i<end;i++)
  { 
    arr[i]=arr[i]*arr[i];
  }
}
int main() 
{
  srand(time(NULL));
  int *arr=(int*)malloc(sizeof(int*)*SIZE);//开始记录时间 
  int64_t beg=GetUs();
  Calc(arr,0,SIZE); //记录结束时间 
  int64_t end=GetUs();
  //两个时间做差，得到Calc执行时间
  printf("time => %ld\n",end-beg);
  return 0;
}
#endif

#if 1
//多线程版本
#define SIZE 100000000
#define THREAD_NUM 4
void Calc(int*arr,int beg,int end)
{ 
  int i=beg;
  for(i;i<end;++i)
  { 
    arr[i]=arr[i]*arr[i];
  }
}
int64_t GetUs()     //获取毫秒
{
  struct timeval tv;
  gettimeofday(&tv,NULL);
  return tv.tv_sec *1000000+tv.tv_usec;
}
typedef struct Arg{
  int beg;
  int end;
  int* arr;
} Arg;
void* ThreadEntry(void *arg)
{
  Arg*p=(Arg*)arg;
  Calc(p->arr,p->beg,p->end);
  return NULL;
}
//线程1 Calc（arr，0，size/2);
//线程2 Calc (arr,size/2,size);

int main()
{
  srand(time(NULL));
  int* arr=(int*)malloc(sizeof(int)*SIZE);
  pthread_t tid[THREAD_NUM];
  Arg args[THREAD_NUM];
  int base=0;
  int i=0;
  for(i=0;i<THREAD_NUM;i++)
  {
    args[i].beg=base;
    args[i].end=base+SIZE/THREAD_NUM;
    args[i].arr=arr;
    base+=SIZE/THREAD_NUM;
  }
  int64_t beg=GetUs();
  int b=0;
  for(b=0;b<THREAD_NUM;++b)
  {
    pthread_create(&tid[b],NULL,ThreadEntry,&args[b]);
  }
  int a=0;
  for(a=0;a<THREAD_NUM;++a)
  {
    pthread_join(tid[a],NULL);
  }
 int64_t end=GetUs();
 printf("time=>%ld\n",end-beg);
  return 0;
}
#endif
