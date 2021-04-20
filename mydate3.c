#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char* argv[])
{
    char buf[BUFSIZ];
    time_t tval;
    struct tm *pTime;

    tval = time(NULL);

    pTime = localtime(&tval);

    if (argc == 1) {
        strftime(buf, sizeof(buf), "%a %b %e %H:%M:%S %Z %Y", pTime);
    }
    else if (argc == 2) {
        if (strcmp(argv[1], "-a") == 0) {
            strftime(buf, sizeof(buf), "%Y %B %e (%A) %l:%M:%S %p, %Z", pTime);
        }
        else if (strcmp(argv[1], "-c") == 0) {
            sprintf(buf, "%s\n", ctime(&tval));
        }
        else {
            exit(-1);
        }
    }
    else if (argc == 3) {
        if (strcmp(argv[1], "-m") == 0) {
            int set_year = atoi(argv[2]);
            sprintf(buf, "%d年1月1日から\n%d年", set_year, pTime->tm_year - set_year + 1900);
            char buf_tmp[1000];
            strftime(buf_tmp, sizeof(buf_tmp), "%m月%e日%H時間%M分%S秒", pTime);
            strcat(buf, buf_tmp);
        }
        else {
            exit(-1);
        }
    }
    else{
        exit(-1);
    }
    printf("%s\n", buf);
}