#include <tk/tkernel.h>  /* T-Kernel ヘッダ */
#include <tm/tmonitor.h> /* tm_printf() など */

#define INT3 3 // INT3の割り込み番号

// GPIOポートA
#define GPIOA_BASE (0x400C0000)
#define PADATA  (GPIOA_BASE + 0x00)
#define PACR    (GPIOA_BASE + 0x04)
#define PAIE    (GPIOA_BASE + 0x38)
#define PAFR3   (GPIOA_BASE + 0x30) // ポートAの機能選択レジスタ

// GPIOポートE
#define GPIOE_BASE (0x400C0400)
#define PEDATA  (GPIOE_BASE + 0x00)
#define PECR    (GPIOE_BASE + 0x04)
#define PEIE    (GPIOE_BASE + 0x38)
#define PECR    (GPIOE_BASE + 0x04) // ポートEの制御レジスタ

// 割り込みハンドラ
void int3_handler(void) {
    // 割り込み処理
    tm_printf("INT3 Interrupt occurred!\n");
    ClearInt(INT3); // 割り込み要因をクリア
}

void init_ports() {
    UINT intsts;
    DI(intsts); // 割り込み禁止

    // PA3ポートの設定
    *(_UW*) PACR &= ~(1<<3); // PA3ポート端子の出力禁止
    *(_UW*) PAIE |= (1<<3);  // PA3ポート端子の入力許可
    *(_UW*) PAFR3 |= (1<<3); // PA3ポート端子をINT3に選択

    // PE3ポートの設定
    *(_UW*)PECR |= (1<<3); // PE3を出力モードに設定

    EI(intsts); // 割り込み許可
}

void setup_interrupt() {
    UINT intsts;
    DI(intsts); // 全ての割り込みを禁止

    SetIntMode(INT3, IM_EDGE | IM_LOW); // 割り込み信号を立下りで検出に設定
    ClearInt(INT3); // 割り込み要因をクリア
    EnableInt(INT3, 5); // 割り込み有効&優先度設定(優先度は0~7)

    EI(intsts); // 割り込み禁止の解除
}

void led_task(INT stacd, void *exinf) {
    *(_UW*)PECR |= (1<<3); // PE3を出力モードに設定

    while (1) {
        *(_UB*)(PEDATA) |= (1 << 3);  // LED点灯
        tk_dly_tsk(500);  // 0.5秒待機
        *(_UB*)(PEDATA) &= ~(1 << 3);  // LED消灯
        tk_dly_tsk(500);  // 0.5秒待機
    }
}

EXPORT INT usermain(void) {
    init_ports(); // ポートの初期化
    setup_interrupt(); // 割り込みの設定

    // タスク生成と起動
    T_CTSK led_ctsk = {
        .tskatr = TA_HLNG | TA_RNG3,
        .task = (FP)led_task,
        .itskpri = 10,
        .stksz = 1024,
        .bufptr = NULL,
        .exinf = NULL
    };
    ID led_tid = tk_cre_tsk(&led_ctsk);
    tk_sta_tsk(led_tid, 0); // LEDタスク開始

    return 0;
}
