#pragma once
#include "tcp_socket.hpp"

class TcpClient{
public:
  TcpClient(){
    sock_.Socket();
  }

  ~TcpClient(){
    sock_.Close();
  }

  bool Connect(const std::string &ip,uint16_t port){  
    return sock_.Connect(ip,port);
  }

  int Recv(std::string*msg){
    return sock_.Recv(msg);
  }

  bool Send(const std::string& msg){
  return sock_.Send(msg);
  }
private:
  TcpSocket sock_;
};
