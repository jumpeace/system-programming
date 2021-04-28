#include <stdio.h>
#include <sys/types.h>
#include <signal.h>

int main(int argc, char const *argv[])
{
    int pid;
    const int PID_MAX = 10000;
    for (pid = 1; pid <= PID_MAX; pid++) {
        int result = kill(pid, 0);
        if (result == 0) {
            printf("%d ", pid);
        }
    }
    return 0;
}
