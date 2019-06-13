#pragma once
#include "socket.hpp"
#include <assert.h>
#include <functional>
//通用udp
//1，读取请求
//2.根据请求计算相应
//3.把相应写回客户端
//2是和业务有关，所以把2对应的代码提取成一个回调函数

//Handler作为一个函数指针
//typedef void(*Handler)(const std::string&req,std::string* resp);

//Handler 函数模板
typedef std::function<void(const std::string&,std::string*)> Handler;

class UdpServer{
public:
  UdpServer(){
    assert(sock_.Socket());
  }

  ~UdpServer(){
    sock_.Close();
  }

  bool Start(const std::string&ip,uint16_t port,Handler handler){
    //1.创建socket 已经在构造函数中创建完成
    //2.绑定端口
    bool ret=sock_.Bind(ip,port);
    if(!ret)
    {
      return false;
    }
    //3.进入一个死循环
    while(true)
    {
      //处理每一个请求
      //a ）读取请求
      std::string req;
      std::string peer_ip;
      uint16_t peer_port;
      sock_.RecvFrom(&req,&peer_ip,&peer_port);
      //b)根据请求计算相应
      std::string resp;
      handler(req,&resp);
      //c）把相应返回客户端
      sock_.SendTo(resp,peer_ip,peer_port);
    }
    return true;
  }
private:
    UdpSocket sock_;
};
