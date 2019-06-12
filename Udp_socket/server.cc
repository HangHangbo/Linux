//先使用UDP版本的服务器
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
int main()
{
  //1.先创建一个socket
  //AF_INET是一个宏，表示ipv4 协议
  //SOCK_DGAM表示UDP协议
  int sock = socket(AF_INET,SOCK_DGRAM,0);
  if(sock<0)
  {
    perror("socket");
  }
  //2.把当前的socket 绑定一个ip+端口号
  sockaddr_in addr;
  addr.sin_family=AF_INET;
  //ip地址也是一个整数，也需要转成网络字节序，只不过inet_addr 自动转换了
  addr.sin_addr.s_addr=inet_addr("0.0.0.0");
  //端口号必须先转成网络字节序
  addr.sin_port=htons(9090);
  int ret=bind(sock,(sockaddr*)&addr,sizeof(addr));
  if(ret<0)
  {
    perror("bind");
    return 1;
  }
  printf("server start ok!\n");
//3.处理服务器收到的请求
  while(1)
  {
    //a  读取客户端请求
    // 面向数据报的函数接口
    char buf[1024]={0};
    sockaddr_in peer;  //表示对端的ip地址和端口号
    socklen_t len=sizeof(peer);//输入输出参数
    ssize_t n=recvfrom(sock,buf,sizeof(buf)-1,0,(sockaddr*)&peer,&len);
    if(n<0)
    {
      perror("recvfrom");
      continue;
    }
    buf[n]='\0';

    //b 根据请求计算相应
    //目前完成一个回显服务器
    //c 把相应写回客户端
    n=sendto(sock,buf,strlen(buf),0,(sockaddr*)&peer,len);
    printf("[%s:%d] buf: %s",inet_ntoa(peer.sin_addr),ntohs(peer.sin_port),buf);
    if(n<0)
    {
      perror("sendto");
      continue;
    }
    //inet_ntoa 将sin_addr转换位点分十进制
    //peer.sin_port是网络字节序，转化为主机字节序
  }
  close(sock);
  return 0;
}
