#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <stdbool.h>

#define R (0)
#define W (1)

int childpid[100] = {0};
int pipe_fd[2];
int argRange[100][2];
int pipeNum = 0;
int bufCurrentPid;
bool isRun = false;
bool showDbg = true;

void catcher()
{
    if (!isRun)
    {
        puts("bye");
        exit(0);
    }

    puts("Process aborted.");
    kill(bufCurrentPid, SIGINT);

    return;
}

void myExec(char *origArg[], int argFrom, int argNum, int currentPipeNum)
{
    int status;

    char *arg[100];
    int i;
    for (i = argFrom; i < argFrom + argNum; i++)
    {
        arg[i - argFrom] = origArg[i];
    }
    arg[i - argFrom] = NULL;

    if (strcmp(arg[0], "cd") == 0)
    {
        if (argNum <= 1)
        {
            //puts("Arg not enough");
            chdir(getenv("HOME"));
        }
        else
        {
            chdir(arg[1]);
        }
    }
    else if (strcmp(arg[0], "exit") == 0)
    {
        exit(0);
    }
    else if (strcmp(arg[0], "showdbg") == 0)
    {
        if (argNum <= 1)
        {
            printf("showDbg: %s\n", showDbg ? "true" : "false");
            return;
        }
        else
        {
            if (strcmp(arg[1], "true") == 0)
            {
                showDbg = true;
            }
            else if (strcmp(arg[1], "false") == 0)
            {
                showDbg = false;
            }
            else
            {
                printf("showDbg: %s\n", showDbg ? "true" : "false");
                return;
            }
            printf("showDbg is now turned %s\n", showDbg ? "on" : "off");
        }
    }
    else
    {
        childpid[currentPipeNum] = fork();
        if (childpid[currentPipeNum] == 0)
        {
            if (currentPipeNum != 0)
            {
                //close(pipe_fd[0]);
                close(STDIN_FILENO);
                dup2(pipe_fd[0], STDIN_FILENO);
            }
            if (currentPipeNum != pipeNum - 1)
            {
                close(STDOUT_FILENO);
                dup2(pipe_fd[1], STDOUT_FILENO);
            }
            if (strlen(arg[0]) <= 1 && (arg[0][0] == 'k' || arg[0][0] == 's' || arg[0][0] == 'c'))
            {
                if (strlen(arg[0]) > 1)
                    return;

                if (argNum <= 1)
                {
                    puts("Arg not enough");
                    exit(1);
                }
                int signalToSend;
                switch (arg[0][0])
                {
                case 'k':
                    signalToSend = SIGINT;
                    break;
                case 's':
                    signalToSend = SIGSTOP;
                case 'c':
                    signalToSend = SIGCONT;
                    break;
                default:
                    puts("Error");
                    exit(0);
                }
                kill(atoi(arg[1]), signalToSend);
                exit(0);
                return;
            }
            else
            {
                execvp(arg[0], &arg[0]);
            }
            exit(123);
        }
        else
        {
            isRun = true;
            bufCurrentPid = childpid[currentPipeNum];
            waitpid(childpid[currentPipeNum], &status, 0);
            printf("%d", status);
            if(status != 0){
                printf("bonysh: command %s does not exist.", arg[0]);
            }
            isRun = false;
        }
    }
}

int main(int argc, char *argv[])
{
    char buffer[256];
    char *tmp = (char *)NULL;
    size_t length;
    char *str_p;
    char *result;
    int status;
    signal(SIGINT, catcher);
    if (pipe(pipe_fd) == -1)
    {
        perror("pipe");
        exit(1);
    }

    while (1)
    {
        char *arg[256];
        char currentDir[256];
        char *test;
        pipeNum = 0;
        getwd(currentDir);
        printf("\n%s >\nΩ ", currentDir);
        if (fgets(buffer, 256, stdin) == NULL || buffer[0] == '\n')
        {
            continue;
        }
        length = strlen(buffer);
        if (buffer[length - 1] == '\n')
        {
            buffer[--length] = '\0';
        }
        if (strlen(buffer) <= 0)
            continue;
        str_p = buffer;
        int argNum;
        for (argNum = 0; (result = strsep(&str_p, " ")) != NULL; argNum++)
        {
            arg[argNum] = result;
            //printf("%s\n", result);
        };
        arg[argNum] = NULL;
        argRange[pipeNum][0] = 0;
        int i;
        for (i = 0; i < argNum; i++)
        {
            if (arg[i][0] == '|')
            {
                //pipe detected
                if (showDbg)
                    puts("detect");
                argRange[pipeNum][1] = i - 1;
                pipeNum += 1;
                argRange[pipeNum][0] = i + 1;
                if (showDbg)
                    puts("");
            }
            else
            {
                if (showDbg)
                    printf("argv[%d] %s\n", i, arg[i]);
            }
        }
        argRange[pipeNum][1] = i - 1;
        pipeNum += 1;
        

        for (i = 0; i < pipeNum; i++)
        {
            myExec(arg, argRange[i][0], argRange[i][1] - argRange[i][0] + 1, i);
        }
    }
}