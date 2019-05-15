#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

//input表示切分命令
//out 表示切分结果（字符串数组）
//返回值表示output 中包含了几个有效元素
int Split(char input[],char* output[])
{
  //借助strtok来实现
  char* p=strtok(input," ");
  int i=0;
  while(p!=NULL){
//    printf("%s\n",p);
   output[i]=p;
   ++i;
    p=strtok(NULL," ");
  }
  return i;
}

int main()
{
  while(1)
  {
  //1.打印一个提示符
  printf("[myshell@localhost ~]$");
  fflush(stdout);
  //2.用户输入指令
  char command[1024]={0};
  gets(command);//一次读一行数据
  //printf("%s\n",command);
  //3.解析命令，把要执行哪个程序
  //那些是命令行参数识别出来，字符串切分（strtok）
  //切分结果应该是一个字符串数组
  char *argv[1024]={0};
  int n=Split(command,argv);
  //4。创建子进程并且程序替换
  CreatProcess();
  }
  return 0;
}
