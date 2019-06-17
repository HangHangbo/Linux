#pragma once
#include <functional>
#include "tcp_socket.hpp"
#include <iostream>
#include <signal.h>

#define CHECK_RET(exp) if (!(exp)){\
  return false;\
}

typedef std::function<void(const std:: string&,std::string*)> Handler;

class TcpProserver{
public:
    TcpProserver(){}
    ~TcpProserver(){
        Listen_sock_.Close();
    }
    bool Start(const std::string &ip,uint16_t port,Handler handler ){
        signal(SIGCHLD,SIG_IGN);
        //1.创建一个socket
        CHECK_RET(Listen_sock_.Socket());
        //printf("Socket ok\n");
        //2.绑定端口号
        CHECK_RET(Listen_sock_.Bind(ip,port));
        //printf("Bind ok\n");
        //3.进入监听模式
        CHECK_RET(Listen_sock_.Listen());
        //printf("Listen ok\n");
        //4.进入主循环
        while(true){
            //5.accept获取一个“链接”
            TcpSocket client_sock;
            std::string peer_ip;
            uint16_t peer_port;
            bool ret=Listen_sock_.Accept(&client_sock,&peer_ip,&peer_port);
            //printf("Accpet ok\n");
            if(!ret){
                perror("Accept");
                continue;
            }
            printf("[%s,%d]有新的客户端链接了！",peer_ip.c_str(),peer_port);
            //6.创建子进程，让子进程处理客户端请求，父进程继续调用 Accept
            Connect_Procerss(client_sock,peer_ip,peer_port,handler);    
        }
    }
        
private:
    TcpSocket Listen_sock_;
    void Connect_Procerss(TcpSocket& client_sock,const std::string &ip,uint16_t port,Handler handler)
    {
        
        //创建子进程
        pid_t ret=fork();
        //父进程直接返回
        if(ret>0){
            //父进程返回到循环里继续调用Accept，每个父进程都会不停的创建socket 不关闭，最终造成文件描述符泄露
            client_sock.Close();
            return;
        }
        //子进程完成和客户端之间的沟通，一次链接多次交互 读写操作
        //a.读取请求
        while(true){
        std::string req;
        int r=client_sock.Recv(&req);
        if(r<0){
            continue;
        }
        if(r==0){
            //对端关闭 
            client_sock.Close();
            printf("[%s:%d]对端关闭了链接\n",ip.c_str(),port);
            break;
        }
        printf("[%s:%d]对端发送了:",ip.c_str(),port);
        std::cout<<req<<std::endl;
        //b.根据请求计算相应
        std::string resp;
        handler(req,&resp);
        //c.将相应返回给客户端
        client_sock.Send(resp);
        }
        //子进程首位工作
        //1.关闭socket
        client_sock.Close();
        //2.结束进程，为了不足造成阻塞等待在每次创建子进程之前接受子进程返回值
        exit(0);
    }
};

