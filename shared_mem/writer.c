#include <string.h>
#include "myshm.h"
int main()
{
  //往共享内存中写数据
  //1.创建/打开共享内存
  int shmid =CreateShm();
  //2.附加到共享内存对象上
  char* p=(char*)shmat(shmid,NULL,0);
  //3.直接使用
  //strcpy(p,"hehe\n");
  printf("输入：");
  char str[1024]={0};
  scanf("%s",str);
  strcpy(p,str);
  return 0;
}
