#include <unordered_map>
//#include "tcp_process_server.hpp"
//#include "tcp_server.hpp"
//#include "tcp_select_server.hpp"
#include "dict_epoll_server.hpp"
int main()
{
  std::unordered_map<std::string,std::string> dict;
  dict.insert(std::make_pair("hello","你好"));
  dict.insert(std::make_pair("world","世界"));
  dict.insert(std::make_pair("bit","比特"));
  //TcpProserver server;
  TcpEpollServer server;
  server.Start("0.0.0.0",9090,[&dict](const std::string& req,std::string *resp)
      {
        auto it=dict.find(req);
        if(it==dict.end())
        *resp="未找到";
        else{
        *resp=it->second;
        }
      });
  return 0;
}
