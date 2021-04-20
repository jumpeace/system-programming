#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int main() {
    // 文字列
    char input[500];
    printf("input> ");
    fgets(input, sizeof(input), stdin);
    input[strlen(input) - 1] = ' ';

    // 文字列を最初の位置に合わせる
    char* str_p = input;
    char separated_strs[500][500];
    int i;
    int str_num = -1;
    for (i = 0; i < 500; i++) {
        char separated_str[500];
        strcpy(separated_str, strsep(&str_p, " "));
        if (strlen(separated_str) == 0) {
            str_num = i;
            break;
        }
        strcpy(separated_strs[i], separated_str);
    }

    if (str_num == -1) {
        exit(-1);
    }
    for (i = 0; i < str_num; i++) {
        printf("[%d] %s\n", i, separated_strs[i]);
    }
}