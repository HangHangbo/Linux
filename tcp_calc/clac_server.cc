#include <sstream>
#include "tcp_server.hpp"

 int main(){
     TcpThreadServer server;
     server.Start("0.0.0.0",9090,[](const std::string&req,std::string *resp)
     {
         
     });
     return 0;
 }