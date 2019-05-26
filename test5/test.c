#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
int main()
{
  printf("hehe\n");
  fprintf(stdout,"haha\n");
  write(1,"maiomaio\n",strlen("maiomiao\n"));
  fork();
  return 0;
}
