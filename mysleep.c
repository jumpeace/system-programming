#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
    if (argc != 2) {
        exit(-1);
    }
    int seconds = atoi(argv[1]);
    alarm(seconds);
    pause();
    return 0;
}
