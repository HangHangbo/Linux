#include <sstream>
#include "tcp_server.hpp"


 int main(){
     TcpThreadServer server;
     server.Start("0.0.0.0",9090,[](const std::string& req,std::string *resp){
         std::stringstream ss;
         int i,j;
         char flog;
         ss<<req;
         ss>>i;
         ss.clear();
         ss<<req;
         ss>>j;
         ss.clear();
         ss<<req;
         ss>>flog;
         if(flog=='+'){
             (*resp).push_back(i+j);
         }
     });
     return 0;
 }