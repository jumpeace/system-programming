#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int childpid;
    int status;

    // この瞬間にプロセスが2つに細胞分裂し、子(新しい)プロセスは fork() から再度実行する
    // fork()の返り値がは子プロセスだったら0, 親(元の)プロセスだったら子プロセス
    childpid = fork();
    if (childpid == 0) {
        // 子プロセス
        printf("child\n");
        exit(0);
    }
    else {
        // 親プロセス
        printf("parent\n");
        wait(&status);
    }
}