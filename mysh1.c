#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int main() {
    char input[1000];
    while(1) {
        printf("prompt> ");
        fgets(input, sizeof(input), stdin);
        input[strlen(input) - 1] = '\0';
        printf("%s\n", input);
    }
}k