#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void print_pid(int child_pid, char* status) {
    printf("[%s] pid = %d, parent pid = %d, childpid = %d\n", status, getpid(), getppid(), child_pid);
}

int main() {
    int childpid;
    int grandchildpid;
    int status;

    // この瞬間にプロセスが2つに細胞分裂し、子(新しい)プロセスはプログラムの最初から実行する
    // fork()の返り値がは子プロセスだったら0, 親(元の)プロセスだったら子プロセス
    childpid = fork();
    if (childpid == 0) {
        // 子プロセス
        grandchildpid = fork();
        if (grandchildpid == 0) {
            // 孫プロセス
            print_pid(grandchildpid, "grandchild");
            exit(0);
        }
        else {
            // 子プロセス
            print_pid(grandchildpid, "child");
            wait(&status);
        }
        exit(0);
    }
    else {
        // 親プロセス
        print_pid(childpid, "parent");
        wait(&status);
    }
}