#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fts.h>
#include <time.h>


void print_date(time_t tVal)
{
    char buf[BUFSIZ];
    struct tm *pTime;

    pTime = localtime(&tVal);

    strftime(buf, sizeof(buf), "%a %b %e %H:%M:%S %Z %Y", pTime);
    printf("%s", buf);
}

int main(int argc, char const *argv[])
{
    FTS *ftsp;
    FTSENT *p;
    char *dotav[] = {".", NULL};

    ftsp = fts_open(dotav, FTS_PHYSICAL, NULL);

    while ((p = fts_read(ftsp)) != NULL)
    {
        printf("-----\n");
        printf("fts_info: %d\n", p->fts_info);
        printf("groupID: %d\n", p->fts_statp->st_gid);
        printf("inode: %ld\n", p->fts_statp->st_ino);
        printf("time of last access: %ld\n", p->fts_statp->st_atime);
        printf("time of last data modification: %ld | ", p->fts_statp->st_mtime);
        print_date(p->fts_statp->st_mtime);
        printf("\ntime of last file status change: %ld\n", p->fts_statp->st_ctime);
        printf("file size: %ld\n", p->fts_statp->st_size);
        printf("number of hard links: %ld\n", p->fts_statp->st_nlink);
        
        switch (p->fts_info)
        {
        case FTS_F:
            printf("filename: %s, uid: %d\n", p->fts_name, p->fts_statp->st_uid);
            break;
        }
    }
}

/*
    [1]FTS_D       preorder でたどられるディレクトリ。
    [2]FTS_DC      ツリーの中で循環しているディレクトリ。 (FTSENT 構造体の fts_cycle フィールドも同様に埋められる。)
    [3]FTS_DEFAULT ファイルタイプを表現する FTSENT 構造体が、 fts_info の他のいずれかの値で明示的に説明されていない。
    [4]FTS_DNR     読み込みができないディレクトリ。 これはエラーの場合の返り値であり、 何がエラーを起こしたかを示すために fts_errno フィールドが設定される。
    [5]FTS_DOT     fts_open()  へのファイル名として指定されなかった "." または ".." という名前のファイル (FTS_SEEDOT を参照すること)。
    [6]FTS_DP      postorder でたどられるディレクトリ。 FTSENT 構造体の内容は、preorder のときに返された状態 (つまり、 fts_info フィールドが FTS_D に設定されている状態) から変更されない。
    [7]FTS_ERR     これはエラーの場合の返り値であり、 fts_errno フィールドは、何がエラーを起こしたかを示す値に設定される。
    [8]FTS_F       通常のファイル。
    [9]FTS_NS      stat(2)  情報が得られなかったファイル。 fts_statp フィールドの内容は定義されない。 これはエラーの場合の返り値であり、 fts_errno フィールドは、何がエラーを起こしたかを示す値に設定される。
    [10]FTS_NSOK    stat(2)  情報が要求されなかったファイル。 fts_statp フィールドの内容は定義されない。
    [11]FTS_SL      シンボリックリンク。
    [12]FTS_SLNONE  リンク先の存在しないシンボリックリンク。 fts_statp フィールドの内容は、シンボリックリンクそのもののファイル特性情報を参照する。
*/