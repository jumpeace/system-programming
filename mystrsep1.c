#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* strsep() : 基準の文字列に従ってstr_pをずらしながら、その間にあった文字列を返す */

int main() {
    // 文字列
    char str[] = "This is a pen.";
    // 文字列内の位置
    char *str_p;

    // 文字列を最初の位置に合わせる
    str_p = str;
    printf("%s\n", strsep(&str_p, " "));
    printf("%s\n", strsep(&str_p, " "));
    printf("%s\n", strsep(&str_p, " "));
    printf("%s\n", strsep(&str_p, " "));
}