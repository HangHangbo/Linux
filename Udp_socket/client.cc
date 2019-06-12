//实现一个UDP版本的客户端
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
//  ./client 127.0.0.1
int main(int argc,char *argv[])
{
  (void)argc;
  //1.先创建一个socket
  int sock=socket(AF_INET,SOCK_DGRAM,0);
  if(sock<0)
  {
    perror("socket");
     return 1;
  }
  //.客户端一般不需要bind
  //bind意味着和某个具体的端口号关联在一起
  //如果没有bind，操作系统就会随机分配一个端口号
  //如果服务器不去bind，就会导致每次启动服务的端口发生变化，客户端就没法链接了
  //如果客户端也bind了，就会出现问题，一个端口号不能有两个进程bind，一个端口号只能bind一个端口号
  //客户端最好还是让操作系统随机分配更科学
  //
  //2.准备一个服务器的sockaddr_in 结构
  sockaddr_in server_addr;
  server_addr.sin_family=AF_INET;
  server_addr.sin_addr.s_addr=inet_addr(argv[1]);
  server_addr.sin_port=htons(9090);
  
  //3.客户端直接发送即可
  while(1)
  {
    char buf[1024]={0};
    printf("输入一段内容：");
    fflush(stdout);
    scanf("%s",buf);
    sendto(sock,buf,strlen(buf),0,(sockaddr*)&server_addr,sizeof(server_addr));
    //从服务器接受返回结果
    char buf_output[1024]={0};
    //后两个参数填NULL,不关注对端地址
    recvfrom(sock,buf_output,sizeof(buf_output)-1,0,NULL,NULL);
    printf("server resp:%s\n",buf_output);
  }
  return 0;
}
