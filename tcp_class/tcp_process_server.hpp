#pragma once
#include <functional>
#include "tcp_socket.hpp"

#include <signal.h>

#define CHECK_RET(exp) if (!(exp)){\
  return false;\
}

typedef std::function<void(const std:: string&,std::string*)> Handler;

class TcpProserver{
public:
    TcpProserver(){}
    bool Start(const std::string &ip,uint16_t port,Handler handler ){
        //1.创建一个socket
        CHECK_RET(Listen_sock_.Socket());
        //2.绑定端口号
        CHECK_RET(Listen_sock_.Bind(ip,port));
        //3.进入监听模式
        CHECK_RET(Listen_sock_.Listen());
        //4.进入主循环
        while(true){
            //5.accept获取一个“链接”
            TcpSocket client_sock;
            std::string peer_ip;
            uint16_t peer_port;
            bool r=Listen_sock_.Accept(&client_sock,&peer_ip,&peer_port);
            if(!r){
                perror("Accept");
                continue;
            }
            printf("[%s,%d]有新的客户端链接了！%s",peer_ip.c_str(),peer_port);
            Connect_Procerss(client_sock,peer_ip,peer_port,handler);    
        }
    }
        
private:
    TcpSocket Listen_sock_;
    void Connect_Procerss(TcpSocket& client_sock,const std::string &ip,uint16_t port,Handler handler)
    {
        signal(SIGCHLD,SIG_IGN);
        //创建子进程
        pid_t pid=fork();
        //父进程直接返回
        if(pid>0){
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
        if(r=0){
            //对端关闭 
            client_sock.Close();
            printf("[%s:%d]对端关闭了链接\n",ip.c_str(),port);
            break;
        }
        printf("[%s:%d]对端发送了：%s",ip.c_str(),port,req.c_str());
        //b.根据请求计算相应
        std::string resp;
        handler(req,&resp);
        //c.将相应返回给客户端
        client_sock.Send(resp);
        }
    }
};

