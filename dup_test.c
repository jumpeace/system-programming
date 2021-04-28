#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    int pipe_fd[2];
    pid_t pid;

    if (pipe(pipe_fd) == -1)
    {
        perror("pipe");
        exit(1);
    }
    /* pipe_fd[0] -- 読み込み用
    * pipe_fd[1] -- 書き込み用
    */
    if ((pid = fork()) == 0)
    {
        char c;
        close(pipe_fd[1]);
        close(0);
        dup(pipe_fd[0]);
        close(pipe_fd[0]);

        while (read(0, &c, sizeof(char)) > 0)
        {
            putchar(c);
        }
    }
    else if (pid > 0)
    {
        char *p;
        close(pipe_fd[0]);
        close(1);
        dup(pipe_fd[1]);
        close(pipe_fd[1]);

        p = "hello,world\n";
        while (*p)
        {
            write(1, p++, sizeof(char));
        }
    }
    else
    {
        perror("fork");
        exit(1);
    }
}
