#pragma once
#include "threadpool.hpp"
#include <stdio.h>
#include <pthread.h>
class Task{
public:
  virtual void Run()
  {
    printf("base");
  }
};

class Threadpool{
public:
  Threadpool(int n)
    :queue_(100) 
    ,worker_count_(n)
  {
    //创建若干线程
    for(int i=0;i<worker_count_;i++)
    {
      pthread_t tid;
      pthread_create(&tid,NULL,ThreadEntry,NULL);
    }
  }

  void AddTask(Task* task) {
  queue_.Push(task);
  }
private:
  BlockingQueue<Task*> queue_;
  int worker_count_;
};
