#include "tcp_client.hpp"

int main(int argc,char* argv[])
{
  if(argc!=2){
    printf("Usage .dict_client [ip]\n");
    return 1;
  }
    TcpClient client;
    bool ret=client.Connect(argv[1],9090);
    if(!ret){
      printf("connect faild!\n");
      return 1;
    }
    printf("连接成功\n");
  while(true){
    //从标准输入读数据
    printf("请输入一段内容：");
    fflush(stdout);
    char req[1024]={0};
    scanf("%s",req);
    //把读到的数据发给服务器
    client.Send(req);
    std::string resp;
    //读取服务器发送的相应
    client.Recv(&resp);
    //把相应结果显示到标准输出上
    printf("resp:%s\n",resp.c_str());
  }
  return 0;
}
