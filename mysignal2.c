#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

void catcher() {
    printf("caught\n");
    exit(0);
}

int main(int argc, char* argv[]) {
    signal(SIGINT, catcher);
    if (argc == 1) {
        while(1) {
            printf("y\n");
        }
    }
    else {
        while(1) {
            printf("%s\n", argv[1]);
        }
    }
}