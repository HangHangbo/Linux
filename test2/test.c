#include <stdio.h>
#include <unistd.h>

int main()
{
  pid_t ret=fork();
  if(ret > 0)
  {
    //father
    int i=0;
    for(i=0;i<5;i++)
    {
      printf("father %d\n",getpid());
      sleep(1);
    }
  }
    else if(ret==0){
      //child
      while(1){
      printf("child %d,%d\n",getpid(),getppid());
      sleep(1);
    }
    }
    else{
      perror("fork");
    } 
  
  return 0;
}
