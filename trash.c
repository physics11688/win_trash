/*
 *  ########## trashコマンドのオプションは以下: ###########
 *
 *  -h  trashの説明とオプションを表示
 *
 *  -v　ゴミ箱に送ったファイルのファイル名を表示
 *
 *  -l ゴミ箱GUI表示
 *
 *  -e ゴミ箱を空に
 *
 *  Author: physics11688 - 15.12.2022
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <windef.h>    // HWNDの定義.
#include <windows.h>   // windows用
#include <shellapi.h>  // SHFileOperationA, SHEmptyRecycleBinA

// 引数
#define help 'h'
#define list 'l'
#define empty 'e'
#define verbose 'v'

uint8_t mvToTrash(char *path);
bool emptyTrash();

// mvToTrashのステータス
enum fileop_Status {
    succes,
    malloc_err,
    can_not_trash,
    terminate,
};

int main(int argc, char *argv[]) {
    char *usage =
        "\n---- ファイルをごみ箱に送るコマンド ----\n\n基本的な使い方: \
trash.exe [ファイル]\n\nオプション:\n\n   \
-h  trashの説明とオプションを表示\n\n   -v　ゴミ箱に送ったファイルのファイル名を表示\n\n   \
-l  ゴミ箱GUI表示\n\n   -e  ゴミ箱を空に";
    uint8_t i;
    int8_t opt;
    bool v;
    bool is_empty;
    opterr = 0;  // 　getopt()のエラーメッセージを無効にする。

    // オプションの解析.
    while ((opt = getopt(argc, argv, "hlev")) != -1) {
        switch (opt) {
            case help:  // helpの表示
                puts(usage);
                return EXIT_SUCCESS;
            case list:  // ゴミ箱のGUI表示
                system("start shell:RecycleBinFolder");
                return EXIT_SUCCESS;
            case empty:  // ゴミ箱を空に
                if ((is_empty = emptyTrash()) == true) {
                    return EXIT_SUCCESS;
                } else {
                    puts("ゴミ箱を空できませんでした");
                    return EXIT_FAILURE;
                }

            case verbose:  // 削除したファイルの表示
                v = true;
                break;
            default:
                puts("存在しないオプションが指定されました");
                puts("run: trash -h");
                return EXIT_FAILURE;
        }
    }


    if (argv[optind] == NULL) {
        puts("削除したいファイルを引数に渡してください");
        return EXIT_SUCCESS;
    }

    // オプション以外の引数の処理
    uint8_t result;
    for (i = optind; i < argc; i++) {
        switch (result = mvToTrash(argv[i])) {
            case malloc_err:
                puts("メモリ確保に失敗しました");
                return EXIT_FAILURE;
            case can_not_trash:
                printf("削除失敗: %s\n", argv[i]);
                return EXIT_FAILURE;
            case terminate:
                puts("trashは強制終了しました");
                return EXIT_FAILURE;

            default:
                if (v == TRUE) {
                    printf("削除: %s\n", argv[i]);
                }
                break;
        }
    }
    return EXIT_SUCCESS;
}

// mvToTrash は path のファイルをゴミ箱($RECYCLE.BIN)へ移動する関数
// pathはNULLではない
uint8_t mvToTrash(char *path) {
    // path + '\0'のメモリ確保
    // strlenの返すバイト長は'\0'を含まないもの
    char *pszFrom = (char *)malloc(strlen(path) + 2 * sizeof('\0'));

    if (pszFrom == NULL) {
        perror("malloc:");
        return malloc_err;
    }

    // pathのコピー . strcpyは \0 を末尾に追加する。
    strcpy(pszFrom, path);

    // SHFileOperationAに渡すFileOp.pFromには終わりを示すために\0\0が末尾に必要
    // mallocで確保したメモリは何で埋められるか環境依存。
    // strcpyは \0 を末尾に追加するし、第一引数の
    // \0もコピーするので空文字追加でいい。
    strcpy(pszFrom + strlen(path) + 1, "");  // ポインタ演算


    /*
     * SHFileOperationAに渡す構造体
     * hWnd              As Long     'フォームのウインドウハンドル
     * wFunc             As Long
     * '処理内容(コピーFO_COPY・削除FO_DELETE・移動FO_MOVE・名前の変更FO_RENAME)
     * pFrom             As Long
     * '元ファイル名(ファイル名の後ろには2つのヌル文字を入れる) pTo As Long
     * '新ファイル名(削除の場合は不要) fFlags            As Integer
     * '動作オプション fAnyOperationsAborted As Long
     * '処理終了前にキャンセルしたときは[1] hNameMappings     As Long
     * 'ファイルネームマッピングオブジェクト lpszProgressTitle As Long
     * 'ダイアログボックスのキャプション
     */
    SHFILEOPSTRUCTA FileOp;
    FileOp.hwnd  = NULL;
    FileOp.wFunc = FO_DELETE;
    // 操作したいファイルの絶対パス。複数操作の場合は\0で区切る
    // 終わりを示すために\0\0が末尾に必要
    FileOp.pFrom = pszFrom;
    // 使わないのでNULL
    FileOp.pTo               = "\0\0";
    FileOp.fFlags            = FOF_SILENT | FOF_ALLOWUNDO | FOF_RENAMEONCOLLISION;
    FileOp.lpszProgressTitle = NULL;


    // ファイルの移動
    int return_code = SHFileOperationA(&FileOp);
    if (pszFrom != NULL) {
        free(pszFrom);
    }

    // 移動失敗
    if (return_code != 0) {
        return can_not_trash;
    }

    // 強制終了
    if (FileOp.fAnyOperationsAborted) {
        return terminate;
    }

    return succes;
}

// ゴミ箱を空にする関数
bool emptyTrash() {
    // ウィンドウハンドルを表わすのに用いるポインタ
    HWND hwd = NULL;

    // pszrootPATH.
    // プロジェクトの設定に応じてcharとwchar_tを使い分けるためにLPCSTR型
    LPCSTR drive = "C:\\";  // ごみ箱のあるルートドライブ名を指定

    /*
     * SHERB_NOCONFIRMATION 削除の確認をしない。
     * SHERB_NOPROGRESSUI   進行状況を表示しない。
     * SHERB_NOSOUND        処理完了時にサウンド無し。
     */
    DWORD flag = SHERB_NOCONFIRMATION | SHERB_NOPROGRESSUI | SHERB_NOSOUND;

    // 成功すると S_OK が返る.失敗すると, OLE定義のエラー値
    HRESULT ok = SHEmptyRecycleBinA(hwd, drive, flag);

    if (ok != S_OK) {
        return false;
    } else {
        return true;
    }
}
