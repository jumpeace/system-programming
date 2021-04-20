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

    if (argc == 2 && strcmp(argv[1], "-a") == 0) {
        strftime(buf, sizeof(buf), "%Y %B %e (%A) %l:%M:%S %p, %Z", pTime);
    }
    else{
        strftime(buf, sizeof(buf), "%a %b %e %H:%M:%S %Z %Y", pTime);
    }
    printf("%s\n", buf);
}