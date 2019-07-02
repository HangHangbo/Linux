#include <sstream>
#include "tcp_client.hpp"
#include <jsoncpp/json/json.h>

#if 0
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
#endif


int main(int argc,char* argv[])
{
  //1.创建有一个Tcp客户端
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
//2.循环从输入读入数据
    while(true){
      printf("请输入两个操作数和一个操作符：");
      fflush(stdout);
      int num1;
      int num2;
      char op[10]={0};
      scanf("%d %d %s",&num1,&num2,op);
    
      //3.根据用户输入的内容构建一个Json对象
      Json::Value req_json;
      req_json["num1"]=num1;
      req_json["num2"]=num2;
      req_json["op"]=op;
      
      //4.把Json对象序列化一个字符串
      Json::StyledWriter writer;
      std::string req=writer.write(req_json);
      printf("req:%s\n",req.c_str());
      //5.把字符串发送到服务器端
      client.Send(req);
      //6.从服务器读取返回的结果
      std::string resp;
      client.Recv(&resp);
      //7.把服务器返回的结果再进行Json解析
      Json::Reader reader;
      Json::Value resp_json;
      reader.parse(resp,resp_json);
      //8.打印结果
      printf("result:%d\n",resp_json["result"].asInt());
    }
    return 0;
}