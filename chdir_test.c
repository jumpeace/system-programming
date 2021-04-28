#include <stdio.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
    if (argc != 2) {
        printf("実行時引数の数が不当です\n");
        return 2;
    }
    if (chdir(*(argv + 1)) != 0) {
        printf("カレントディレクトリの変更に失敗しました\n");
        return 1;
    }
    printf("カレントディレクトリの変更に成功しました\n");
    char cur_dir[1000] = "";
    getwd(cur_dir);
    printf("current_dir: %s", cur_dir);

    return 0;
}