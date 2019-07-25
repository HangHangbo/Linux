#pragma once
#include <vector>
#include <set>
#include <functional>
#include <sys/select.h>
#include "tcp_socket.hpp"

bool operator<(const TcpSocket &lhs,const TcpSocket &rhs){
    return lhs.GetFd()<rhs.GetFd();
}
//select这个函数用起来不太方便，封装起来成一个类
//提供以下几个操作
//文件描述符的管理器,这个管理器中包含的socket 就是要使用select来管理的socket
//1.新增一个文件描述符
//2.删除一个文件描述符
//3.等待操作 等待管理的文件描述符就绪（只考虑读就绪的情况）

class Selector{
public:
    void Add(const TcpSocket &sock){
        printf("[Selector::Add] %d\n",sock.GetFd());
        socket_set_.insert(sock);
    }

    void Del(const TcpSocket &sock){
        printf("[Select::Del] %d\n",sock.GetFd());
        socket_set_.erase(sock);
    }

    //Wait返回的时候 要告诉调用者那些文件描述符就绪了
    void Wait(std::vector<TcpSocket> *output){
        output->clear();
        //调用Wait就相当于调用select进行等待
        //先获取到最大的文件描述符
        if(socket_set_.empty()){
            printf("[Selector::Wait] socket_set_ is empty\n");
            return;
        }
        int max_fd=socket_set_.rbegin()->GetFd();
        //申请一个位图
        fd_set readfds;
        //初始化全0
        FD_ZERO(&readfds);
        //将set 中的每一个需要关注的文件描述符都设置到readfds中
        for(const auto& sock :socket_set_ ){
            FD_SET(sock.GetFd(),&readfds);
        }
        //select 默认是一个阻塞等待，有文件描述符返回的时候才会返回
        //当这个函数返回之后，就应该根据文件描述符的返回情况
        //构造一个输出参数，告诉调用者有哪些socket 就绪了
        int nfds=select(max_fd+1,&readfds,NULL,NULL,NULL);
        if(nfds<0){
            //selsect 失败
            perror("select");
            return;
        }
        
        //已经返回了，返回结果同一保存在 readfds 之中
        for(int fd=0;fd<max_fd+1;fd++){
            if(FD_ISSET(fd,&readfds)){
                //如果是，说明当前fd就是读就绪的
                //就把这个结果放到输出结果中
                output->push_back(TcpSocket(fd));
            }
        }
    }
private:
    //这里需要一个数据结构把需要的socket存取来
    //需要高效的插入删除，排列有序，删除的时候不会发送数据搬移,方便找大最大值
    std::set<TcpSocket> socket_set_;
};

//实现一个select版本的TCP server

#define CHECK_RET(exp)\
    if(!exp){\
        return false;\
    }

typedef std::function<void(const std::string&,std::string*)> Handler;

class TcpSelectServer{
public:
    //一个服务器程序在处理请求的时候四暗星的流程就是三步
    //1.读取请求响应
    //2.根据请求计算相应（handler）
    //3.把相应写回到客户端
    bool Start(const std::string& ip,uint16_t port,Handler handler){
        //1.创建Socket
        TcpSocket listen_sock;
        CHECK_RET(listen_sock.Socket());
        //2.绑定端口号
        CHECK_RET(listen_sock.Bind(ip,port));
        //3.监听文件描述符
        CHECK_RET(listen_sock.Listen());
        //4.创建一个Selector对象，让这个对象先把 listen_sock 管理器起来，后续进行等待，都是靠selector 对象完成
        Selector selector;
        selector.Add(listen_sock);
        //5.进入主循环
        while(true){
            //不再直接调用 accept ，而不是使用select进行等待
            std::vector<TcpSocket> output;
            selector.Wait(&output);
            //7.遍历返回结果，依次处理每一个就绪的socket
            for(auto tcp_socket:output){
                //8.分成两种情况
                    //a.如果就绪的socket 是listen_socket 要进行的操作的调用accept
                    if(tcp_socket.GetFd()==listen_sock.GetFd()){
                        TcpSocket client_sock;
                        std::string ip;
                        uint16_t port;
                        tcp_socket.Accept(&client_sock,&ip,&port);
                        selector.Add(client_sock);
                        printf("[client %s:%d] connected!\n",ip.c_str(),port);
                    }
                    else{
                    //b.如果就绪的socket不是Listen_ocket 要进行的操作是直接调用 recv
                        std::string req;
                        int n=tcp_socket.Recv(&req);
                        if(n<0){
                            continue;
                        }
                        if(n==0){
                            //对端关闭，也要将这个socket从selector 中删掉
                            tcp_socket.Close();
                            selector.Del(tcp_socket);
                            continue;
                        }
                        printf("[client] %s\n",req.c_str());
                        std::string resp;
                        handler(req,&resp);
                        tcp_socket.Send(resp);
                    }
            }
        }
    }

private:

};