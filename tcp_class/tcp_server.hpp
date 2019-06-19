//通用的TCP的服务器

#pragma once
#include <functional>
#include "tcp_socket.hpp"
#include <iostream>
#define CHECK_RET(exp) if (!(exp)){\
  return false;\
}
typedef std::function<void(const std:: string&,std::string*)> Handler;

class TcpServer{
public:
  TcpServer(){}
  bool Start(const std::string& ip,uint16_t port,Handler handler){
    //1.创建一个socket
    CHECK_RET(listen_sock_.Socket());
    //2.绑定端口号
    int ret=listen_sock_.Bind(ip,port);
    if(!ret){
      return false;
    }
    //3.进行监听
    CHECK_RET(listen_sock_.Listen());
    //4.进入主循环
    while(true){
      //5.通过accept获取到一个连接
      TcpSocket client_sock;
      std::string ip;
      uint16_t port;
     bool ret= listen_sock_.Accept(&client_sock,&ip,&port);
     if(!ret){
       continue;
     }
     printf("[%s:%d]有客户端连接\n",ip.c_str(),port);
      //6.和客户端进行沟通，一次连接进行多次交互  读写操作
     while(true){
       //a）读取请求
       std::string req;
       int r=client_sock.Recv(&req);
       if(r<0){
         continue;
       }
       if(r==0){
      //对端关闭了 socket
         client_sock.Close();
         printf("[%s:%d]对端关闭了连接\n",ip.c_str(),port);
         break;
       }
       printf("[%s:%d] 客户端发送了",ip.c_str(),port);
       std::cout<<req<<std::endl;
    //b）根据请求计算相应
      std::string resp;
      handler(req,&resp);
    //c)把响应写回客户端
      client_sock.Send(resp);
     }
    }
  }
private:
  TcpSocket listen_sock_;
};
