#pragma once
#include <functional>
#include <pthread.h>
#include "tcp_socket.hpp"

#define CHECK_RET(exp) if (!(exp)){\
  return false;\
}

typedef std::function<void(const std:: string&,std::string*)> Handler;

class TcpThreadServer{
public:
    TcpThreadServer(){

    }
    ~TcpThreadServer(){

    }

    bool Start(const std::string&ip,uint16_t port,Handler handler){
        //1.创建socket
        CHECK_RET(Listen_sock_.Socket());
        //2.绑定端口号
        CHECK_RET(Listen_sock_.Bind(ip,port));
        //3.进入监听模式
        CHECK_RET(Listen_sock_.Listen());
        //4.进入主循环
        while(true){
            //5.调用accept
            TcpSocket client_sock;
            std::string peer_ip;
            uint16_t peer_port;
            bool ret=Listen_sock_.Accept(&client_sock,&peer_ip,&peer_port);
            if(!ret){
                perror("Accept");
                continue;
            }
            printf("[%s:%d]有新的客户端链接了！",peer_ip.c_str(),peer_port);
            //6.创建线程处理函数的逻辑
            ProcessConnect(client_sock,peer_ip,peer_port,handler);
    }
}

private:
    TcpSocket Listen_sock_;
    struct ThreadEntryArg{
        TcpSocket client_sock;
        std::string ip;
        uint16_t port;
        Handler hangdler;
    };
    void ProcessConnect(TcpSocket &client_sock,const std::string &ip,uint16_t port,Handler handler){
        //1.创建线程
        pthread_t tid;
        //这样申请的结构体作用域只是在这个大括号里面，当主线程执行完了，这个结构体也就释放了，新的线程访问这个结构图回访问越界
        //ThreadEntryArg arg;
        //加上static后 修改生命周期为全局变量，主线程结束不会释放内存，但是考虑到每个线程都会修改这个结构内容，并且线程的执行顺序是抢占式执行的，就会造成线程访问出错
        //static ThreadEntryArg arg;
        //综上所述又要在主线程声明周期结束的术后这个结构体不被释放，又要让每一个线程都有属于自己的一份，只能在堆上申请了
        ThreadEntryArg *arg=new ThreadEntryArg;
        arg->client_sock=client_sock;
        arg->hangdler=handler;
        arg->ip=ip;
        arg->port=port;
        pthread_create(&tid,NULL,ThreadEntry,(void*)arg);
        //2.对著线程来说，让函数立即返回
        pthread_detach(tid);
        //3.对于新新城来说，循环处理客户端的操作
    }
    static void* ThreadEntry(void *arg){
            ThreadEntryArg* argument=(ThreadEntryArg*) arg;
            TcpSocket &client_sock=argument->client_sock;
            uint16_t& port=argument->port;
            std::string ip=argument->ip;
            Handler handler=argument->hangdler;
            while(true){
            //a)读取客户请求
            std::string req;
            int ret=client_sock.Recv(&req);
            if(ret<0){
                continue;
            }
            if(ret==0){
                client_sock.Close();
                printf("[%s:%d] 客户端断开连接\n",ip.c_str(),port);
                break;
            }
            //b）根据请求计算响应
            printf("[%s:%d]客户端发送了：%s\n",ip.c_str(),port,req.c_str());
            std::string resp;
            handler(req,&resp);
            //c）返回响应
            client_sock.Send(resp);
        }
        delete argument;
    }
};
