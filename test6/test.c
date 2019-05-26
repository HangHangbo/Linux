#include <stdio.h>
//函数声明
extern int add(int a,int b);
int main()
{
  int ret=add(10,20);
  printf("%d\n",ret);
  return 0;
}
