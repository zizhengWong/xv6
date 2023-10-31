#include "kernel/types.h"

#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"

void find(char *path, const char *filename)
{
  char buf[512], *p;
  int fd;
  struct dirent de; //表示目录项的数据结构
  struct stat st; //表示文件状态信息的数据结构

  if ((fd = open(path, 0)) < 0) { //打开文件
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0) { //获取打开的文件的状态信息
    fprintf(2, "find: cannot fstat %s\n", path);
    close(fd);
    return;
  }

  //参数错误，find的第一个参数必须是目录
  if (st.type != T_DIR) {
    fprintf(2, "usage: find <DIRECTORY> <filename>\n");
    return;
  }

  if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) { //DIRSIZ表示文件名的最大长度，+1是为了\0
    fprintf(2, "find: path too long\n");
    return;
  }
  // printf("path: %s\n",path); //在终端看看path是什么样子的，就是目录的路径（相对路径）
  strcpy(buf, path);
  p = buf + strlen(buf); //移动p指针
  *p++ = '/'; //添加一个'/'
  while (read(fd, &de, sizeof de) == sizeof de) {
    if (de.inum == 0) //inode号为0通常是一个无效的值，表示一个未分配的inode或一个已删除的文件
      continue;
    memmove(p, de.name, DIRSIZ); //添加路径名称，将 de.name 中的数据复制到指针 p 指向的内存位置，复制的数据长度是 DIRSIZ 字节。
    p[DIRSIZ] = 0;               //字符串结束标志，p原地偏移DIRSIZ的位置，但p不动，现在p相当于只有一个文件名
    if (stat(buf, &st) < 0) {
      fprintf(2, "find: cannot stat %s\n", buf);
      continue;
    }
    //不要在“.”和“..”目录中递归
    if (st.type == T_DIR && strcmp(p, ".") != 0 && strcmp(p, "..") != 0) {
      find(buf, filename);
    } else if (strcmp(filename, p) == 0){
      // printf("p: %s\n",p); //可以在终端看到文件名输出
      printf("%s\n", buf);
    }
  }

  close(fd);
}

int main(int argc, char *argv[])
{
  if (argc != 3) {
    fprintf(2, "usage: find <directory> <filename>\n");
    exit(1);
  }
  find(argv[1], argv[2]);
  exit(0);
}
