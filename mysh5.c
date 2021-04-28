#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <stdbool.h>

int child_pid;

int len_str_array(char* str_array[]) {
    int i;
    for (i = 0;; i++) {
        if (str_array[i] == NULL) {
            return i;
        }
    }
}

void catcher()
{
    if (child_pid == 0) {
        exit(0);
    }
    else {
        kill(child_pid, 0);
    }
}

void print_prompt() {
    char cur_dir[100] = "";
    getcwd(cur_dir, sizeof(cur_dir));
    printf("prompt %s > ", cur_dir);
}

int cd(char* argv[]) {
    int argc = len_str_array(argv);
    char path[100] = "";
    if (argc == 2) {
        strcpy(path, argv[1]);
    }
    else if (argc == 1) {
        strcpy(path, getenv("HOME"));
    }
    if (chdir(path) != 0) {
        printf("カレントディレクトリの変更に失敗しました\n");
    }
}

int main() {
    char buf[1000];
    char **ap, *argv[10], *inputstring;
    int i;
    int status;

    print_prompt();
    signal(SIGINT, catcher);
    while(fgets(buf, 1000, stdin) != NULL) {
        buf[strlen(buf) - 1] = '\0';


        inputstring = buf;
        for (ap = argv; (*ap = strsep(&inputstring, " ")) != NULL;)
            if (**ap != '\0')
                if (++ap >= &argv[10])
                    break;

        signal(SIGINT, catcher);
        child_pid = fork();
        if (child_pid == 0)
        {
            if (strcmp(argv[0], "k") == 0) {
                kill(atoi(argv[0]), SIGINT);
            }
            else if (strcmp(argv[0], "s") == 0) {
                kill(atoi(argv[0]), SIGSTOP);
            }
            else if (strcmp(argv[0], "c") == 0) {
                kill(atoi(argv[0]), SIGCONT);
                return 0;
            }
            else if (strcmp(argv[0], "cd") == 0) {
                exit(257);
            }
            else {
                execv(argv[0], argv);
            }
            exit(0);
        }
        else
        {
            wait(&status);
            if (status == 256) {
                cd(argv);
            }
            printf("[status] <16:%04X> <10:%04d>\n", status, status);
            print_prompt();
        }
    }
}