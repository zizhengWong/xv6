#include "kernel/types.h"
#include "user/user.h"

#define RD 0
#define WR 1

const uint INT_LEN = sizeof(int);

/**
 * @brief 处理第一个数据
 * @param lpipe_RD 左邻居的读权限
 * @param pfirst 用于存储第一个数据的地址
 * @return 如果没有数据返回-1,有数据返回0
 */
int lpipe_first_data(int lpipe_RD, int *dst)
{
  if (read(lpipe_RD, dst, sizeof(int)) == sizeof(int)) {
    printf("prime %d\n", *dst);
    return 0;
  }
  return -1;
}

/**
 * @brief 读取左邻居剩下的数据，将不能被first整除的写入右邻居
 * @param lpipe_RD 左邻居的读权限
 * @param rpipe 右邻居的管道符
 * @param first 左邻居的第一个数据
 */
void transmit_data(int lpipe_RD, int rpipe[2], int first) // int rpipe[2]等价于int rpipe[]，传的是指针
{
  int data;
  // 从左管道读取数据
  while (read(lpipe_RD, &data, sizeof(int)) == sizeof(int)) {
    // 将无法整除的数据传递入右管道
    if (data % first)
      write(rpipe[WR], &data, sizeof(int));
  }
  close(lpipe_RD);//从左边读取完毕
  close(rpipe[WR]);//向右边写入完毕
}

/**
 * @brief 寻找素数
 * @param lpipe 左邻居管道
 */
void primes(int lpipe[2])
{
  close(lpipe[WR]); //关闭自己对左边管道的写
  int first;
  if (lpipe_first_data(lpipe[RD], &first) == 0) {
    int rpipe[2];
    pipe(rpipe); // 建立右边的管道
    transmit_data(lpipe[RD], rpipe, first);

    if (fork() == 0) {
      primes(rpipe);    // 递归的思想，这将在一个新的进程中调用
    } else {
      close(rpipe[RD]); //自己没必要读，但之前要先保留着读权限给子进程继承，现在关了；写权限在transmit_data里关了
      wait(0);
    }
  }
  exit(0);
}

int main(int argc, char const *argv[])
{
  int p[2];
  pipe(p);

  for (int i = 2; i <= 35; ++i) //写入初始数据
    write(p[WR], &i, INT_LEN);

  if (fork() == 0) {
    primes(p);
  } else {
    close(p[WR]);
    close(p[RD]);
    wait(0); //参数为0时，父进程会等待任何子进程退出。
  }

  exit(0);
}
