#pragma once

#include <vector>
#include <pthread.h>
#include <semaphore.h>

//一般的blockingqueue都要求长度是有上限的
//如果队列为空，去执行pop就会阻塞
//如果队列满了，去执行push就会阻塞
//信号量表示可用资源的个数
//另一个信号量表示当前队列中空格的个数
//
//插入元素就是在消耗一个空格资源（V），释放一个元素资源（P）
//删除元素就是在消耗一个元素资源（P）,释放了一个空格资源（V）
//如果用信号量表示互斥，P 和V 操作在同一个函数中
//如果用信号量表示同步，P和V操作在不同的函数中
//
//P 操作和 V操作都是原子操作

template <typename T>
class BlockingQueue{
public:
  BlockingQueue(int max_size)
    :max_size_(max_size)
    ,head_(0)
     ,tail_(0)
     ,size_(0)
     ,queue_(max_size)
  {
    sem_init(&lock_,0,1);
    sem_init(&elem_,0,0);
    sem_init(&blank_,0,max_size);
  }
  ~BlockingQueue()
  {
    sem_destroy(&lock_);
    sem_destroy(&elem_);
    sem_destroy(&blank_);
  }
  void Push(const T& data)
  {
    //每次要插入元素，就得先申请一个空格的资源
    //如果没有空格的资源（信号量为0），说明队列满了
    //满了就不能继续插入了，并且在push中阻塞
    sem_wait(&blank_);

    sem_wait(&lock_);
      queue_[tail_]=data;
      ++tail_;
      ++size_;
      sem_post(&lock_);

      sem_post(&elem_);
  }
  void Pop(T* data)
  {
    //每次出队列的时候，就先申请一个元素资源
    //如果没有元素的资源（信号量为0），说明队列为空
    //就不能直接进入队列，而需要在pop的位置阻塞
    sem_wait(&elem_);
    sem_wait(&lock_);
    *data=queue_[head_];
    ++head_;
    --size_;
    sem_post(&lock_);
    sem_post(&blank_);
  }
private:
  std::vector<T> queue_;
  int head_;
  int tail_;
  int size_;
  int max_size_;
  sem_t lock_; //使用一个二元信号量（非0即1）表示互斥锁
  sem_t elem_;//可使用元素的个数
  sem_t blank_;//可以空格的个数
};
