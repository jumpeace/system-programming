#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <fts.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdbool.h>

// バイナリファイルのfts_info
#define FTS_B 101

// 表示色
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"
#define DEFAULT_COLOR "\x1b[39m"
// 表示形式
#define BOLD "\x1b[1m"
#define DEFAULT_TYPE "\x1b[0m"

int child_pid;

int len_str_array(char *str_array[])
{
    int i;
    for (i = 0; str_array[i] != NULL; i++)
        ;
    return i;
}

int cd(char *argv[])
{
    int argc = len_str_array(argv);
    char path[FILENAME_MAX] = "";
    if (argc == 2)
        strcpy(path, argv[1]);
    else if (argc == 1)
        strcpy(path, getenv("HOME"));
    if (chdir(path) != 0)
        printf("カレントディレクトリの変更に失敗しました\n");
}

int is_binaryfile(const char *filename)
{
    FILE *fp;
    int c;

    fp = fopen(filename, "rb");

    while (feof(fp) == 0)
    {
        c = fgetc(fp);
        if (c == EOF)
            break;
        // ASCIIコードの08H以下が検出されたらおそらくバイナリファイル
        if (c <= 8)
        {
            fclose(fp);
            return true;
        }
    }
    fclose(fp);
    return false;
}

int is_hidden(const char *filename)
{
    return filename[0] == '.';
}

void ls(char *argv[])
{
    int argc = len_str_array(argv);

    // モードを判定
    typedef enum Mode {Normal, F, Other} Mode;
    Mode mode;
    if (argc == 1)
        mode = Normal;
    else if (argc == 2) {
        if (strcmp(argv[1], "-F") == 0) 
            mode = F;
        else 
            mode = Other;
    }
    else 
        mode = Other;
    
    // モードが不適切が場合
    if (mode == Other) {
        printf("ls:引数が不適切です。\n");
        return;
    }

    /* ftsの準備 */
    FTS *ftsp;
    FTSENT *p;
    char *dotav[] = {".", NULL};
    ftsp = fts_open(dotav, FTS_PHYSICAL, NULL);

    /* ファイル名を取得 & ファイルの種類を取得 */
    char fts_name_list[100][FILENAME_MAX];
    int info_list[100];
    int is_first = true;
    int surface_directory_inode = -1;
    int print_fts_count = 0;
    while ((p = fts_read(ftsp)) != NULL)
    {
        // 最初はlsするディレクトリ自体を返すため飛ばす
        if (is_first)
        {
            is_first = false;
            continue;
        }
        // ディレクトリ内を見ている場合
        if (surface_directory_inode != -1)
        {
            // ディレクトリ内を見終わった場合
            if (p->fts_info == FTS_DP && p->fts_statp->st_ino == surface_directory_inode)
                surface_directory_inode = -1;
            continue;
        }
        // ディレクトリの場合
        if (p->fts_info == FTS_D)
        {
            surface_directory_inode = p->fts_statp->st_ino;
        }
        // 隠しファイルやディレクトリの場合
        if (is_hidden(p->fts_name)) 
            continue;

        switch (p->fts_info)
        {
        case FTS_F:
            sprintf(fts_name_list[print_fts_count], "%s", p->fts_name);
            if (is_binaryfile(fts_name_list[print_fts_count]))
            {
                info_list[print_fts_count] = FTS_B;
            }
            else
            {
                info_list[print_fts_count] = FTS_F;
            }
            print_fts_count++;
            break;
        case FTS_D:
            sprintf(fts_name_list[print_fts_count], "%s", p->fts_name);
            info_list[print_fts_count] = FTS_D;
            print_fts_count++;
            break;
        case FTS_SL:
            sprintf(fts_name_list[print_fts_count], "%s", p->fts_name);
            info_list[print_fts_count] = FTS_SL;
            print_fts_count++;
            break;
        }
    }

    /* ファイルやディレクトリをアルファベット順にソート */
    int i, j, k;
    int lower, upper;
    char lower_c, upper_c;
    int is_swap;
    char tmp_str[FILENAME_MAX];
    int tmp_num;
    for (i = 0; i < print_fts_count - 1; i++)
    {
        for (j = print_fts_count - 1; j >= i + 1; j--)
        {
            lower = j - 1;
            upper = j;
            is_swap = false;

            // 交換するかどうかを判定
            for (k = 0;; k++)
            {
                lower_c = fts_name_list[lower][k];
                upper_c = fts_name_list[upper][k];
                if (lower_c == '\0')
                    break;
                else if (upper_c == '\0')
                {
                    is_swap = true;
                    break;
                }
                if (lower_c > upper_c)
                {
                    is_swap = true;
                    break;
                }
                else if (lower_c < upper_c)
                    break;
            }
            // 交換操作
            if (is_swap)
            {
                strcpy(tmp_str, fts_name_list[lower]);
                strcpy(fts_name_list[lower], fts_name_list[upper]);
                strcpy(fts_name_list[upper], tmp_str);

                tmp_num = info_list[lower];
                info_list[lower] = info_list[upper];
                info_list[upper] = tmp_num;
            }
        }
    }

    /* ファイルやディレクトリを表示 */
    for (i = 0; i < print_fts_count; i++)
    {
        switch (info_list[i])
        {
        case FTS_B:
            printf("%s%s", GREEN, BOLD);
            break;
        case FTS_D:
            printf("%s%s", BLUE, BOLD);
            break;
        case FTS_SL:
            printf("%s%s", CYAN, BOLD);
            break;
        }
        printf("%s%s%s", fts_name_list[i], DEFAULT_COLOR, DEFAULT_TYPE);
        if (mode == F)
        {
            switch (info_list[i])
            {
            case FTS_B:
                printf("*");
                break;
            case FTS_D:
                printf("/");
                break;
            case FTS_SL:
                printf("@");
                break;
            }
        }
        printf("  ");
    }
    printf("\n");
}

void print_prompt()
{
    char cur_dir[100] = "";
    getcwd(cur_dir, sizeof(cur_dir));
    printf("%s%s%s%sΩσ%s%s ", MAGENTA, BOLD, cur_dir, YELLOW, DEFAULT_COLOR, DEFAULT_TYPE);
}

void print_status(int status){
    printf("\n%sSTATUS: %04X%s\n", YELLOW, status, DEFAULT_COLOR);
}

void catcher()
{
    if (child_pid == 0)
        exit(0);
    else
        kill(child_pid, 0);
}

int main()
{
    char buf[128];
    char *argv[10], *inputstring;
    int pipe_p;
    int i;
    int status;
    int argc = 0;
    int pipe_fd[2];

    if (pipe(pipe_fd) == -1)
    {
        perror("pipe");
        exit(1);
    }

    print_prompt();
    while (fgets(buf, 1000, stdin) != NULL)
    {
        buf[strlen(buf) - 1] = '\0';

        inputstring = buf;
        pipe_p = -1;

        for (i = 0;; i++)
        {
            argv[i] = strsep(&inputstring, " ");
            if (inputstring == NULL)
            {
                argv[i + 1] = (char *)NULL;
                break;
            }
            if (strcmp(argv[i], "|") == 0)
            {
                argv[i] = (char *)NULL;
                pipe_p = i;
            }
        }
        argc = i;

        if (strcmp(argv[0], "k") == 0)
        {
            kill(atoi(argv[0]), SIGINT);
            print_prompt();
        }
        else if (strcmp(argv[0], "s") == 0)
        {
            kill(atoi(argv[0]), SIGSTOP);
            print_prompt();
        }
        else if (strcmp(argv[0], "c") == 0)
        {
            kill(atoi(argv[0]), SIGCONT);
            print_prompt();
        }
        else if (strcmp(argv[0], "cd") == 0)
        {
            cd(argv);
            print_prompt();
        }
        else if (strcmp(argv[0], "ls") == 0)
        {
            ls(argv);
            print_prompt();
        }
        else
        {
            // パイプなし
            if (pipe_p == -1)
            {
                signal(SIGINT, catcher);
                if ((child_pid = fork()) == -1)
                {
                    perror("Can't fork");
                    exit(1);
                }

                if (child_pid == 0)
                    execvp(argv[0], argv);
                else
                {
                    wait(&status);
                    print_status(status);
                    print_prompt();
                }
            }
            // パイプあり
            else
            {
                signal(SIGINT, catcher);
                if ((child_pid = fork()) == -1)
                {
                    perror("Can't fork");
                    exit(1);
                }

                if (child_pid == 0)
                {
                    dup2(pipe_fd[1], STDOUT_FILENO);
                    execvp(argv[0], argv);
                }
                else
                {
                    wait(&status);
                    if (status != 0)
                    {
                        print_status(status);
                        print_prompt();
                    }
                    else
                    {
                        signal(SIGINT, catcher);
                        if ((child_pid = fork()) == -1)
                        {
                            perror("Can't fork");
                            exit(1);
                        }

                        if (child_pid == 0)
                        {
                            dup2(pipe_fd[0], STDIN_FILENO);
                            execvp(argv[pipe_p + 1], &argv[pipe_p + 1]);
                        }
                        else
                        {
                            wait(&status);
                            print_status(status);
                            print_prompt();
                        }
                    }
                }
            }
        }
    }
}