#include <sstream>
#include "tcp_client.hpp"

int main(int argc,char* argv[])
{
  if(argc!=2){
    printf("Usage .calc_client [ip]\n");
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
    int num1=0;
    int num2=0;
    char op[10];
    scanf("%d %d %s",&num1,&num2,op);

    //序列化数据
    std::stringstream ss;
    ss<<num1<<" "<<num2<<" "<<op;
    
    //把读到的数据发给服务器
    client.Send(ss.str());
    std::string resp;
    //读取服务器发送的相应
    client.Recv(&resp);
    //把相应结果显示到标准输出上
    printf("resp:%s\n",resp.c_str());
  }
  return 0;
}