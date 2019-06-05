#pragma once
#include "blockingqueue.hpp"

#include <stdio.h>

class Task{
  public:
    virtual void Run()
    {
      printf("base Run\n");
    }
};

class ThreadPool{
  public:
    ThreadPool(int n) :queue_(100),work_count_(n),workers_(0)
  {
    //创建出，若干个线程
   pthread_t tid;
   pthread_create(&tid,NULL,ThreadEntry,this);
   workers_.push_back(tid);
  }
    
  ~ThreadPool()
  {
    for(ssize_t i=0;i<workers_.size();i++)
    {
      pthread_cancel(workers_[i]);
    }
    for(size_t i=0;i<workers_.size();i++)
    {
     pthread_join(workers_[i],NULL);
    }
  
  }
    //使用线程池的时候，就需要由调用者加入一下任务，让线程取执行
    void AddTask(Task* task)
    {

    }
  private:
  BlockingQueue<Task*> queue_; 
  int work_count_;
  std::vector<pthread_t> workers_;//用来记录每个线程的tid 用于析构

  static void*ThreadEntry(void* arg)
  {
    ThreadPool* pool=(ThreadPool*)arg;
    while(true)
    {
      //循环中尝试从阻塞队列中获取任务
      //获取到一个任务就执行一个任务

    }
    return NULL;
  }
};
