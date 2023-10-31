#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char* argv[]) {
    if (argc - 1 >= MAXARG) {
        fprintf(2, "xargs: too many arguments.\n");
        exit(1);
    }
    else if(argc < 2) {
        printf("Usage: xargs [command]\n");
        exit(1);
    }
    int i, count = 0, k, m = 0;
    char* lineSplit[MAXARG], *p;
    char block[32], buf[32];
    p = buf;
    // 先把xrags后面的复制过来
    for (i = 1; i < argc; i++) {
        lineSplit[count++] = argv[i];
    }
    // 先用block接，然后用buf分割好，再交给lineSplit
    while ((k = read(0, block, sizeof(block))) > 0) {
        for (i = 0; i < k; i++) {
            if (block[i] == '\n') {
                // 开始一条命令
                buf[m] = 0;
                lineSplit[count++] = p;
                // =0用做隔离，并重新初始化
                lineSplit[count] = 0;
                m = 0;
                p = buf;
                count = argc - 1;

                if (fork() == 0) {
                    exec(argv[1], lineSplit);
                }else{
                    wait(0);
                }
            } else if (block[i] == ' ') {
                // 读入一个参数
                buf[m++] = 0;
                lineSplit[count++] = p;
                p = &buf[m];
            } else {
                buf[m++] = block[i];
            }
        }
    }
    exit(0);
}
