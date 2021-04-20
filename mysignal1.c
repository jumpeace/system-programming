#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void catcher() {
    printf("caught\n");
    exit(0);
}

int main() {
    signal(SIGINT, catcher);
    while(1) {
        printf("y\n");
    }
}