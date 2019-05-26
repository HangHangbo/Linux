#include <stdio.h>
#include <unistd.h>
#include <string.h>
int main()
{
  //使用pipe函数创建一对文件描述符，通过这一对文件描述符就能操作内核中的管道
  int fd[2];
  int ret = pipe(fd);
  if(ret<0)   //文件描述符已经用完了
  {
    perror("pipe");
    return 1;
  }
  //fd[0] ->读数据
  //fd[1] ->写数据
  //char buf[1024]="hehe";
  //write(fd[1],buf,strlen(buf));
  //char buf_output[1024]={0};
  //ssize_t n = read(fd[0],buf_output,sizeof(buf_output)-1);
  //buf_output[n]='\0'; 
 // printf("%s\n",buf_output);
  //管道使用完毕后，需要及时关闭文件描述符
   ret=fork();
   if(ret>0)
   {
     //father 写数据
     char buf[1024]="hehe";
     write(fd[1],buf,strlen(buf));
     sleep(1);//此时子进程读取到
     char buf_output[1024]={0}; //管道中的数据一旦被读取，就相当出队列了，只能读一次   
     read(fd[0],buf_output,sizeof(buf_output)-1);
     printf("father read %s\n",buf_output);
   }
   else if(ret==0)
   {
     //child  读数据
     char buf_output[1024]={0};
    read(fd[0],buf_output,sizeof(buf_output)-1);
     printf("chile read %s\n",buf_output);
   }
   else
   {
     perror("fork");
   }
  close(fd[0]);
  close(fd[1]);
  return 0;
}

