#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>


void run_command() {
    char input[1000];
    printf("prompt> ");
    fgets(input, sizeof(input), stdin);
    input[strlen(input) - 1] = '\0';
    execl(input, input, (char*)0);
}

void fork_process() {
    int childpid;
    int status;
    childpid = fork();
    if (childpid == 0) {
        // 子プロセス
        run_command();
    }
    else {
        // 親プロセス
        wait(&status);
        // 子プロセスに記述すると処理後にプロセスが終了してしまうので親プロセスに記述
        fork_process();
    }
}

int main() {
    fork_process();
}