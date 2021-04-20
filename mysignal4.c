#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

void catcher1() {
    printf("caught1\n");
}

void catcher2() {
    printf("caught2\n");
    exit(0);
}

int main(int argc, char* argv[]) {
    /* Ctrl + C */
    signal(SIGINT, catcher1);
    /* Ctrl + \ */
    signal(SIGQUIT, catcher2);
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