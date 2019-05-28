#pragma once
#include <stdio.h>
#include <unistd.h>
#include <sys/shm.h>
static int CreateShm(){
//int main(){
  key_t key=ftok(".",0x1);
  if(key==-1)
  {
    perror("ftok");
    return 1;
  }
  printf("key=%d\n",key);

  int ret=shmget(key,1024,IPC_CREAT | 0666);
  if(ret<0){
    perror("shmget");
    return 1;
  }
  printf("ret=%d\n",ret);
  return ret;
}
