#include "kernel/types.h" //虽然这里没用到，但是不加这个头文件，用到的其他函数报类型错误
#include "user/user.h"

int main(int argc, char const *argv[])
{
  if (argc != 2) { //参数错误
    fprintf(2, "usage: sleep <time>\n");
    exit(1);
  }
  sleep(atoi(argv[1]));
  exit(0);
}