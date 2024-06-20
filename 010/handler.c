#include <tk/tkernel.h>
#include <tm/tmonitor.h>

// アラームハンドラ関数の定義
void almhdr(void *exinf) {
    // アラームハンドラの処理内容
    tm_printf("Alarm handler executed!\n");
    return;
}

int usermain(void) {
    T_CALM calm;    // アラームハンドラ生成情報
    ID almid;       // アラームハンドラID番号

    // アラームハンドラ生成情報の設定
    calm.almatr = TA_HLNG; // 高級言語で記述されている
    calm.almhdr = almhdr;  // アラームハンドラのアドレス

    // アラームハンドラの生成
    almid = tk_cre_alm(&calm);
    if (almid < E_OK) {
        // アラームハンドラ生成に失敗
        tm_printf("Failed to create alarm handler: %d\n", almid);
        return almid;
    }

    // アラームハンドラの動作開始
    ER ercd = tk_sta_alm(almid, 3000); // 3000ミリ秒後に実行
    if (ercd < E_OK) {
        // アラームハンドラ動作開始に失敗
        tm_printf("Failed to start alarm handler: %d\n", ercd);
        return ercd;
    }

    // アラームハンドラが実行されるのを待つ
    tk_slp_tsk(TMO_FEVR);

    return 0;
}
