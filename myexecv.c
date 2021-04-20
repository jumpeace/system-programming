#include <unistd.h>

int main(int argc, char* argv[]) {
    // &argv[k] ... 要素番号がk以上の配列を出力
    execv(argv[1], &argv[1]);
}