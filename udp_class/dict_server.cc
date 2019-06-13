//创建一个中英文翻译的服务器
//用户输入一个英文单词，返回一个翻译出来的的结果
#include "udp_server.hpp"
#include <unordered_map>

int main()
{
        std::unordered_map<std::string,std::string> dict;
        dict.insert(std::make_pair("hello","你好"));
        dict.insert(std::make_pair("world","世界"));
        dict.insert(std::make_pair("bit","比特"));
  UdpServer server;
  //lambda 表达式的[]中可以写东西
  //把lambda 外部的变量“捕获”到lambda 表达式
  //捕获可以安值来捕获，也可以按引用捕获（更高效）
  server.Start("0.0.0.0",9090,[&dict](const std::string& req,std::string* resp)
      { 
       auto it= dict.find(req);
        if(it==dict.end())
        {
        *resp="未找到结果";
        }
        *resp=it->second;
      });
  return 0;
}

