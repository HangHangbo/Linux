#include <sstream>
#include "tcp_server.hpp"

#if 0
//请求
//“10 20 +”
//响应
//“30”

 int main(){
     TcpThreadServer server;
     server.Start("0.0.0.0",9090,[](const std::string& req,std::string *resp){
         //1.反序列化
         std::stringstream ss(req);
         int num1=0;
         int num2=0;
         ss>>num1;
         ss>>num2;
         std::string op;
         ss>>op;
         printf("req:%d %d %s\n",num1,num2,op.c_str());
         //2.进行计算
        int ret=0;
        if(op=="+"){
            ret=num1+num2;
        }
        else if(op=="-"){
            ret=num1-num2;
        }
        else if(op=="*"){
            ret=num1*num2;
        }
        else if(op=="/"){
            ret=num1/num2;
        }
        else if(op=="%"){
            ret=num1%num2;
        }
        //3.把结果序列化
        //*resp=std::to_string(ret);
        std::stringstream ss_resp;
        ss_resp<<ret;
        *resp=ss_resp.str();
     });
     return 0;
 }
 #endif
//使用Josn的方式进行序列化和化反序列化
//{ 
//  “num”：10
//  “num”：20
//  “op”：+  
//}
//解析JOSN格式
//借助第三方库，直接完成解析和操作
#include <jsoncpp/json/json.h>
int main(){
    TcpThreadServer server;
    server.Start("0.0.0.0",9090,[](const std::string &req,std::string *resp){
        //1.先将req进行反序列化

        Json::Reader reader;        //用来表示一个Json数据
        Json::Value req_json;       //把字符串转换成Json::Value
        
        reader.parse(req,req_json);
        int num1=req_json["num1"].asInt();
        int num2=req_json["num2"].asInt();
        std::string op=req_json["op"].asString();
        int ret=0;
        if(op=="+"){
            ret=num1+num2;
        }
        else if(op=="-"){
            ret=num1-num2;
        }
        else if(op=="*"){
            ret=num1*num2;
        }
        else if(op=="/"){
            ret=num1/num2;
        }
        else if(op=="%"){
            ret=num1%num2;
        }
        //3.需要将结果再序列化回一个字符串
        
        Json::Value resp_json;      //把Json::Value 转成字符串
        Json::FastWriter writer;
        
        resp_json["result"]=ret;
        *resp=writer.write(resp_json);
    });
    return 0;
}
