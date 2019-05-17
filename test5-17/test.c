#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
int main(){
  //printf("%d\n",stdin->_fileno);
 // printf("%d\n",stdout->_fileno);
  //printf("%d\n",stderr->_fileno);
  //int fp=open("./test.txt",O_WRONLY | O_CREAT); //只写打开 或没有这个文件就创建这个文件  | 在这里时按位或
  //printf("fp=%d\n",fp);
  //if(fp<0)
  //{
  //  perror("open");
  //  return 1;
  //}
  //读文件
  //char buf[1024]={0};
  //ssize_t n=read(fp,buf,sizeof(buf)-1);
 // buf[n]='\0';
  //printf("%s\n",buf);
  
  //写文件
  //char buf[1024]="hehe";
  //ssize_t n=write(fp,buf,strlen(buf));
  //printf("%ld\n",n);
  //close(fp);
  //bint i=0;
  //for(i;i<2;i++)
  //{
  //  fork();
  //  printf("=");
  //  fflush(stdout);
 // }
 pid_t ret=fork();
 if(ret>0){
   //father
   printf("father %d\n",getpid());
   int status=0;
   pid_t result=wait(&status);  //status 是一个输出型参数
   printf("result %d\n",result);
 }
 else if(ret==0)
 {
   //child
   int count=3;
   while(count>0)
   {
     printf("child %d\n",getpid());
     sleep(1);
     --count;
   }
   exit(0);
 }
 else{
   perror("fork");
 }
  return 0;
}
