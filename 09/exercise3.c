#include <tk/tkernel.h>
#include <tm/tmonitor.h>

// GPIOポートA SW3
#define GPIOA_BASE (0x400C0000)
#define PADATA  (GPIOA_BASE + 0x00)
#define PACR    (GPIOA_BASE + 0x04)
#define PAIE    (GPIOA_BASE + 0x38)
#define PAFR3   (GPIOA_BASE + 0x3c)

#define INT3    3 // INT3の割り込み番号

#define PEDATA 0x400C0400 // PEデータレジスタのアドレス
#define PECR   0x400C0404 // PE出力コントロールレジスタ

ID cycid; // 周期ハンドラID
BOOL is_blinking = FALSE; // 点滅状態を保持するフラグ

void cychdr(void *exinf) {
    // 周期ハンドラの処理内容
    static BOOL is_on = FALSE;
    *(_UW*)PECR |= (1 << 3); //PE3出力許可
    while(1){
        tk_slp_tsk(TMO_FEVR); //タスクを起床待ちに
        if (is_on) {
            tk_stp_cyc(cycid); //周期ハンドラ停止
            *(_UB*)(PEDATA) &= ~(1 << 3); // LEDオフ
            is_on = FALSE;
        } else {
            tk_sta_cyc(cycid); //周期ハンドラ実行
            is_on = TRUE;
        }
        return; // 周期ハンドラの終了
    }
    
}

void inthdr(UINT intno) {
    ClearInt(INT3);

    if (is_blinking) {
        tk_stp_cyc(cycid); // 周期ハンドラを停止
        *(_UB*)(PEDATA) &= ~(1 << 3); // LEDオフ
        is_blinking = FALSE;
    } else {
        tk_sta_cyc(cycid); // 周期ハンドラを開始
        is_blinking = TRUE;
    }
    return;
}

EXPORT int usermain(void) {
    T_CCYC ccyc; // 周期ハンドラ生成情報
    T_DINT dint; // 割り込みハンドラ定義情報
    UINT intsts;

    // PE3を出力に設定
    *(_UW*)PECR |= (1 << 3);

    // 周期ハンドラ設定
    ccyc.cycatr = TA_HLNG;
    ccyc.exinf = 0;
    ccyc.cychdr = cychdr;
    ccyc.cyctim = 1000; // 1秒周期
    ccyc.cycphs = 0;
    cycid = tk_cre_cyc(&ccyc);

    DI(intsts); // 全ての割り込みを禁止

    // 割り込みハンドラ定義
    dint.intatr = TA_HLNG; // 割り込みハンドラ属性
    dint.inthdr = inthdr; // 割り込みハンドラの実行アドレス
    tk_def_int(INT3, &dint); // 割り込みハンドラの定義

    // GPIO設定
    *(_UW*)PACR &= ~(1 << 3); // PA3を入力に設定
    *(_UW*)PAIE |= (1 << 3); // PA3の割り込み許可
    *(_UW*)PAFR3 &= ~(1 << 3); // PA3を通常のGPIOとして設定
    SetIntMode(INT3, IM_EDGE | IM_LOW); // 割り込み信号を立下りで検出に設定

    ClearInt(INT3); // 割り込み要因クリア
    EnableInt(INT3, 5); // 割り込み有効&優先度設定(優先度は0~7)

    EI(intsts); // DIによる割り込み禁止の解除

    while (1) {
        tk_slp_tsk(TMO_FEVR); // 無限待機
    }

    return 0;
}
