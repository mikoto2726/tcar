#include <tk/tkernel.h>  /* T-Kernel ヘッダ */
#include <tm/tmonitor.h> /* tm_printf() など */

/* GPIO(Port-E) */
#define PEDATA      0x400C0400     // Data Register
#define PECR        0x400C0404     // Output Control register
#define PEIE        0x400C0438     // Input Control register

/* LEDのポート */
#define LED_PORT 3

/* 周期ハンドラ関数の定義 */
void cychdr(void *exinf) {
    static BOOL is_on = FALSE; // LEDのON/OFF状態

    if (is_on) {
        *(_UB*)(PEDATA) &= ~(1 << LED_PORT); // LED OFF
        is_on = FALSE;
    } else {
        *(_UB*)(PEDATA) |= (1 << LED_PORT); // LED ON
        is_on = TRUE;
    }
}

int usermain(void) {
    T_CCYC ccyc;    // 周期ハンドラ生成情報
    ID cycid;       // 周期ハンドラID番号

    // PE3出力許可
    *(_UW*)PECR |= (1 << LED_PORT);

    // 周期ハンドラ生成情報の設定
    ccyc.cycatr = TA_HLNG | TA_STA; // 生成後、すぐに動作状態に
    ccyc.cychdr = cychdr;           // 周期ハンドラのアドレス
    ccyc.cyctim = 1000;             // 1秒周期で実行
    ccyc.cycphs = 1000;             // 生成後、1秒後に実行

    // 周期ハンドラの生成
    cycid = tk_cre_cyc(&ccyc);
    if (cycid < E_OK) {
        // 周期ハンドラ生成に失敗
        tm_printf("Failed to create cyclic handler: %d\n", cycid);
        return cycid;
    }

    // メインタスクはスリープ状態にして、周期ハンドラが実行されるのを待つ
    tk_slp_tsk(TMO_FEVR);

    return 0;
}
