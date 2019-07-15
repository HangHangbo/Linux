#pragma once
#include <sys/epoll.h>
#include <vector>
#include <functional>
#include "tcp_socket.hpp"

//Epoll这个类就对标Selector这个类
class Epoll{
public:
    Epoll(){
        epoll_fd_=epoll_create(1);
    }

    ~Epoll(){

    }
    void Add(const TcpSocket &sock){
        printf("[Epoll:Add] %d\n",sock.GetFd());
        epoll_event event;
        event.events=EPOLLIN;
        //此处epoll add 的时候插入的是键值对
        //fd在键和值之间都出现了，这件事情也是迫不得已（epoll的槽点）
        event.data.fd=sock.GetFd();
        epoll_ctl(epoll_fd_,EPOLL_CTL_ADD,sock.GetFd(),&event);
    }

    void Del(const TcpSocket &sock){
        printf("[Epoll:Del] %d\n",sock.GetFd());
        epoll_ctl(epoll_fd_,EPOLL_CTL_DEL,sock.GetFd(),NULL);
    }

    void Wait(std::vector<TcpSocket>* output){
        output->clear();
        //等待文件描述符
        epoll_event events[100];
        //最后一个参数表示阻塞等待
        //返回之后，就有若干个文件描述符就绪，保存在events数组之中
        //返回值结果就是在描述数组中有几个有效元素
        //epoll_wait 返回的只是键值对的值
        //如果这里不加任何处理的话，用户不知道对于文件描述符是谁
        //迫不得已，只能在插入的时候，把socket往值里也存了一份
        int nfds=epoll_wait(epoll_fd_,events,100,-1);
        if(nfds<0){
            perror("epoll_wait");
            return;
        }
        //依次处理每个就绪的文件描述符
        for(int i=0;i<nfds;i++){
            int sock =events[i].data.fd;
            output->push_back(TcpSocket(sock));
        }

    }
private:
    //通过这个epoll_fd_,找到内核中的对象，从而进行文件描述符的管理
    int epoll_fd_;
};

#define CHECK_RET(exp)\
    if(!exp){\
        return false;\
    }

typedef std::function<void(const std::string&,std::string*)> Handler;

class TcpEpollServer{
public:
    bool Start(const std::string &ip,uint16_t port,Handler handler){
        //1.创建socket
        TcpSocket listen_sock;
        CHECK_RET(listen_sock.Socket());
        //2.绑定端口号
        CHECK_RET(listen_sock.Bind(ip,port));
        //3.监听socket
        CHECK_RET(listen_sock.Listen());
        //4.创建Epoll 对象，并把 listen_sock 用 Epoll 管理起来
        Epoll epoll;
        epoll.Add(listen_sock);
        //5.进入主循环
        while(true){
        //6.使用Epoll::Wait 等待文件描述符就绪
        std::vector<TcpSocket> output;
        epoll.Wait(&output);
        //7.循环处理每个就绪的文件描述符，也就是分成两种情况
        for(auto sock:output){
            if(sock.GetFd()==listen_sock.GetFd()){
                //a.listen_sock,调用Accept
                TcpSocket client_sock;
                sock.Accept(&client_sock);
                epoll.Add(client_sock);
            }
                else{
                //b.非listen_sock,就调用Recv
                    std::string req;
                    int n=sock.Recv(&req);
                    if(n<0){
                        continue;
                    }
                    if(n==0){
                    //对端关闭
                        sock.Close();
                        epoll.Del(sock);
                    }
                //正确读取的情况
                std::string resp;
                handler(req,&resp);
                sock.Send(resp);
                }
            }     
        }
    }
};