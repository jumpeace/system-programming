#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/uio.h>

int main(int argc, char const *argv[])
{
    int fd, pipe_fd[2];
    char buf[128];
    int childpid, status;

    fd = open("mypipe2.c", O_RDONLY);

    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(1);
    }

    if ((childpid = fork()) == -1) {
        perror("Can't fork");
        exit(1);
    }

    // parent
    if (childpid != 0) {
        read(fd, buf, sizeof(buf));
        write(pipe_fd[1], buf, sizeof(buf));
        wait(&status);
    }
    else {
        read(pipe_fd[0], buf, sizeof(buf));
        write(1, buf, sizeof(buf));
    }

    return 0;
}
