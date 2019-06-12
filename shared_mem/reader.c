#include "myshm.h"

int main()
{
  //从共享内存读取数据
  //1.创建/打开共享内存
  int shmid=CreateShm();
  //2.附加到共享内存上
  char* p=(char*)shmat(shmid,NULL,0);
  //3.直接使用
  printf("reader:%s\n",p);
  return 0;
}
