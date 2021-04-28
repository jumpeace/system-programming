#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>


void fork_process() {
    int pid;
    int status;
    char *str_p;
    char buf[1000];
    char *strs[1000];
    int i;
    pid = fork();
    if (pid == 0) {
        printf("prompt> ");
        fgets(buf, 1000, stdin);
        buf[strlen(buf) - 1] = '\0';

        // 単語分割
        str_p = buf;
        for (i = 0; str_p; i++)
            strs[i] = strsep(&str_p, " ");
        execv(strs[0], strs);
    }
    else {
        wait(&status);
        printf("[status] %04x\n", status);
        fork_process();
    }
}

int main(){
    fork_process();
}