#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <vector>
//实现一个交易尝试
//实现一个交易场所

#if 0
 std::vector<int> data;
 pthread_mutex_t lock;
 pthread_cond_t cond;
void* Product(void* arg)
{
  //负责把数据插入到数组中
  (void) arg;
  int count=0;
  while(1)
  {
    pthread_mutex_lock(&lock);
    data.push_back(++count);
    pthread_mutex_unlock(&lock);
    pthread_cond_signal(&cond);
    usleep(789789);
  }
  return NULL;
}
void* Consume(void* arg)
{
  //负责从数组中取元素
  (void) arg;
  while(1)
  {
    pthread_mutex_lock(&lock);
    while(data.empty())  //这里最好用while而不是if，pthread_cond_wait 返回不一定就是线程 signal，这个函数也可能被其他信号打断
    {
      //1.先释放锁
      //2.等待条件就绪（有其他线程调用pthread_cond_signal）
      //3.如果条件就绪了，重新获取锁
      //4.加上wait 之后最大的意义在于如果没有数据，消费者线程就不洗进行空转节省了资源
      pthread_cond_wait(&cond,&lock);
    }
   int result= data.back();
    data.pop_back();
    printf("result=> %d\n",result);
    
    pthread_mutex_unlock(&lock);
    usleep(123123);
  }
  return NULL;
}
 //然后又两个角色，生产者和消费者两个线程
int main()
{
  pthread_mutex_init(&lock,NULL);
  pthread_cond_init(&cond,NULL);
  pthread_t tid1,tid2,tid3,tid4;
  pthread_create(&tid1,NULL,Product,NULL);
  pthread_create(&tid3,NULL,Product,NULL);
  pthread_create(&tid2,NULL,Consume,NULL);
  pthread_create(&tid4,NULL,Consume,NULL);
  pthread_join(tid1,NULL);
  pthread_join(tid3,NULL);
  pthread_join(tid2,NULL);
  pthread_join(tid4,NULL);
  pthread_mutex_destroy(&lock);
  pthread_cond_destroy(&cond);
  return 0;
}
#endif

#if 0
#include "blockingqueue.hpp"
BlockingQueue<int> queue(100);

void* Product(void* arg)
{
  (void)arg;
  int count=0;
  while(1)
  {
    queue.Push(++count);
    usleep(789789);
  }
  return NULL;
}
void* Consume(void*arg)
{
  (void)arg;
  while(1)
  {
    int count=0;
    queue.Pop(&count);
    printf("count=> %d\n",count);
    usleep(123123);
  }
  return NULL;
}
int main()
{
  pthread_t tid1,tid2;
  pthread_create(&tid1,NULL,Product,NULL);
  pthread_create(&tid2,NULL,Consume,NULL);
  pthread_join(tid1,NULL);
  pthread_join(tid2,NULL);
  return 0;
}
#endif
#if 1
#include "threadpool.hpp"
//这个类由用户自定义，需要依赖那些数据可以添加修改
class MyTask:public Task{
  public:
    MyTask(int id) :id_(id){

    }
    void Run()
    {
      //执行用户自定制的逻辑
      printf("id=%d\n",id_);
    }
  private:
    int id_;
};
int main()
{
  ThreadPool pool(10);
  for(int i=0;i<20;++i)
  {
    pool.AddTask(new MyTask(i);)
  }
  return 0;
}
#endif















