#include "udp_client.hpp"
#include <iostream>
int main(int argv,char* argc[]){
  (void)argv;
  UdpClient client(argc[1],9090);
  while(1){
    printf("输入一段内容:");
    fflush(stdout);
    char buf[1024*10]={0};
    scanf("%s",buf);
    client.SendTo(buf);
    std::string output;
    client.RecvFrom(&output);
    std::cout<<"server resp:"<<output<<std::endl;
  }

  return 0;
}
