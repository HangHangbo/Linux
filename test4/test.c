#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
int main()
{
  //重定向，依赖操作系统底层行为
  close(1);
  int fd=open("test.txt",O_WRONLY);
  fprintf(stderr,"fd=%d\n",fd);
  printf("%d\n",stdout->_fileno);
  fprintf(stdout,"%d\n",stdout->_fileno);
  return 0;
}
