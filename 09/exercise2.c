#include <tk/tkernel.h>
#include <tm/tmonitor.h>

#define PE_DATA 0x400C0400 // PEデータレジスタのアドレス
#define PE_CR   0x400C0404 // PE出力コントロールレジスタ

void cychdr(void *exinf) {
    // 周期ハンドラの処理内容
    static BOOL is_on = FALSE;

    if (is_on) {
        *(_UB*)(PE_DATA)&= ~(1 << 3); // LEDオフ
        is_on = FALSE;
    } else {
        *(_UB*)(PE_DATA) |= (1 << 3); // LEDオン
        is_on = TRUE;
    }
    return; // 周期ハンドラの終了
}

EXPORT INT usermain(void) {
    T_CCYC ccyc; // 周期ハンドラ生成情報
    ID cycid; // 周期ハンドラID

    *(_UW*)PE_CR |= (1 << 3); // PE3を出力に設定

    ccyc.cycatr = TA_HLNG | TA_STA;
    ccyc.cychdr = cychdr;
    ccyc.cyctim = 500;
    ccyc. cycphs = 500;
    cycid = tk_cre_cyc(&ccyc);
    tk_slp_tsk(TMO_FEVR);// アラームハンドラが実行されるのを待つ
    return 0;
}
