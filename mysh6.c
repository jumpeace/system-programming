#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>

int child_pid;

int len_str_array(char *str_array[])
{
    int i;
    for (i = 0; str_array[i] != NULL; i++)
        ;
    return i;
}

void catcher()
{
    if (child_pid == 0)
        exit(0);
    else
        kill(child_pid, 0);
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
    if (argc == 2)
        strcpy(path, argv[1]);
    else if (argc == 1)
        strcpy(path, getenv("HOME"));
    if (chdir(path) != 0)
        printf("カレントディレクトリの変更に失敗しました\n");
}

int main()
{
    char buf[128];
    char *argv[10], *inputstring;
    int pipe_p;
    int i;
    int status;
    int argc = 0;
    int pipe_fd[2];

    if (pipe(pipe_fd) == -1)
    {
        perror("pipe");
        exit(1);
    }

    print_prompt();
    while (fgets(buf, 1000, stdin) != NULL)
    {
        buf[strlen(buf) - 1] = '\0';

        inputstring = buf;
        pipe_p = -1;

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

        if (strcmp(argv[0], "k") == 0)
        {
            kill(atoi(argv[0]), SIGINT);
            print_prompt();
        }
        else if (strcmp(argv[0], "s") == 0)
        {
            kill(atoi(argv[0]), SIGSTOP);
            print_prompt();
        }
        else if (strcmp(argv[0], "c") == 0)
        {
            kill(atoi(argv[0]), SIGCONT);
            print_prompt();
        }
        else if (strcmp(argv[0], "cd") == 0)
        {
            cd(argv);
            print_prompt();
        }
        else
        {
            // パイプなし
            if (pipe_p == -1)
            {
                signal(SIGINT, catcher);
                if ((child_pid = fork()) == -1)
                {
                    perror("Can't fork");
                    exit(1);
                }

                if (child_pid == 0)
                    execvp(argv[0], argv);
                else
                {
                    wait(&status);
                    printf("status: %04X\n", status);
                    print_prompt();
                }
            }
            // パイプあり
            else
            {
                signal(SIGINT, catcher);
                if ((child_pid = fork()) == -1)
                {
                    perror("Can't fork");
                    exit(1);
                }

                if (child_pid == 0)
                {
                    dup2(pipe_fd[1], STDOUT_FILENO);
                    execvp(argv[0], argv);
                }
                else
                {
                    wait(&status);
                    if (status != 0)
                    {
                        printf("status: %04X\n", status);
                        print_prompt();
                    }
                    else
                    {
                        signal(SIGINT, catcher);
                        if ((child_pid = fork()) == -1)
                        {
                            perror("Can't fork");
                            exit(1);
                        }

                        if (child_pid == 0)
                        {
                            dup2(pipe_fd[0], STDIN_FILENO);
                            execvp(argv[pipe_p + 1], &argv[pipe_p + 1]);
                        }
                        else
                        {
                            wait(&status);
                            printf("status: %04X\n", status);
                            print_prompt();
                        }
                    }
                }
            }
        }
    }
}