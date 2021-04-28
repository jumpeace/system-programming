#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <stdbool.h>

int child1_pid, child2_pid;

int len_str_array(char *str_array[])
{
    int i;
    for (i = 0; str_array[i] != NULL; i++)
        ;
    return i;
}

void catcher1()
{
    if (child1_pid == 0)
    {
        exit(0);
    }
    else
    {
        kill(child1_pid, 0);
    }
}

void catcher2()
{
    if (child2_pid == 0)
    {
        exit(0);
    }
    else
    {
        kill(child2_pid, 0);
    }
}

void print_prompt()
{
    char cur_dir[100] = "";
    getcwd(cur_dir, sizeof(cur_dir));
    printf("prompt %s > ", cur_dir);
}

int cd(char *argv[])
{
    int argc = len_str_array(argv);
    char path[100] = "";
    printf("argc: %d\n", argc);
    if (argc == 2)
    {
        strcpy(path, argv[1]);
    }
    else if (argc == 1)
    {
        strcpy(path, getenv("HOME"));
    }
    printf("path: %s\n", path);
    if (chdir(path) != 0)
    {
        printf("カレントディレクトリの変更に失敗しました\n");
    }
}

int main()
{
    char buf[1000];
    char *argv[10], *inputstring;
    int pipe_p;
    int i;
    int status1, status2;
    int argc = 0;

    print_prompt();
    while (fgets(buf, 1000, stdin) != NULL)
    {
        buf[strlen(buf) - 1] = '\0';

        inputstring = buf;
        pipe_p = -1;

        // コマンドの入力をクリア
        for (i = 0; i < argc; i++)
            argv[i] = (char *)NULL;

        for (i = 0;; i++)
        {
            argv[i] = strsep(&inputstring, " ");
            if (inputstring == NULL)
            {
                argv[i + 1] = (char *)NULL;
                break;
            }
            if (strcmp(argv[i], "|") == 0)
            {
                argv[i] = (char *)NULL;
                pipe_p = i;
            }
        }
        argc = i;
        // for (ap = argv; ap < &argv[10] && (*ap = strsep(&inputstring, " ")) != NULL; ap++) {
        //     if (**ap == '\0')
        //         continue;
        //     if (strcmp(*ap, "|") == 0) {
        //         *ap = (char*)NULL;
        //     }
        // }

        printf("pipe_p: %d\n", pipe_p);

        for (i = 0; i < 2; i++)
        {
            printf("argv[%d]: %s\n", i, *(argv + i));
        }

        signal(SIGINT, catcher1);
        child1_pid = fork();
        if (pipe_p != -1)
        {
            signal(SIGINT, catcher2);
            child2_pid = fork();
        }
        if (child1_pid == 0)
        {
            if (strcmp(argv[0], "k") == 0)
            {
                kill(atoi(argv[0]), SIGINT);
            }
            else if (strcmp(argv[0], "s") == 0)
            {
                kill(atoi(argv[0]), SIGSTOP);
            }
            else if (strcmp(argv[0], "c") == 0)
            {
                kill(atoi(argv[0]), SIGCONT);
                return 0;
            }
            else if (strcmp(argv[0], "cd") == 0)
            {
                exit(257);
            }
            else
            {
                execvp(argv[0], argv);
            }
            exit(0);
        }
        else if (pipe_p != -1 && child2_pid == 0)
        {
            waitpid(child1_pid, &status1, 0);
            if (status1 == 256)
            {
                cd(argv);
            }
            printf("[status1] <16:%04X> <10:%04d>\n", status1, status1);

            int base_p = pipe_p + 1;
            if (strcmp(argv[base_p], "k") == 0)
            {
                kill(atoi(argv[base_p]), SIGINT);
            }
            else if (strcmp(argv[base_p], "s") == 0)
            {
                kill(atoi(argv[base_p]), SIGSTOP);
            }
            else if (strcmp(argv[base_p], "c") == 0)
            {
                kill(atoi(argv[base_p]), SIGCONT);
                return 0;
            }
            else if (strcmp(argv[base_p], "cd") == 0)
            {
                exit(257);
            }
            else
            {
                execvp(argv[base_p], &argv[base_p]);
            }
            exit(0);
        }
        else
        {
            // パイプなし
            if (pipe_p == -1)
            {
                waitpid(child1_pid, &status1, 0);
                if (status1 == 256)
                {
                    cd(argv);
                }
                printf("[status1] <16:%04X> <10:%04d>\n", status1, status1);
            }
            // パイプあり
            else
            {
                printf("0303");
                // FIXME child2_pidを待ってくれない
                waitpid(child2_pid, &status2, 0);
                printf("0505");
                if (status2 == 256)
                {
                    cd(argv);
                }
                printf("[status2] <16:%04X> <10:%04d>\n", status2, status2);
            }
            print_prompt();
        }
    }
}