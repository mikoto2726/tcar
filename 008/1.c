#include <tk/tkernel.h>  /* T-Kernel ヘッダ */
#include <tm/tmonitor.h> /* tm_printf() など */

#define INT3 3 // INT3の割り込み番号

// GPIOポートA
#define GPIOA_BASE (0x400C0000)
#define PADATA  (GPIOA_BASE + 0x00)
#define PACR    (GPIOA_BASE + 0x04)
#define PAIE    (GPIOA_BASE + 0x38)
#define PAFR3    (GPIOA_BASE + 0x010)


// GPIOポートE
#define GPIOE_BASE (0x400C0400)
#define PEDATA  (GPIOE_BASE + 0x00)
#define PECR    (GPIOE_BASE + 0x04)
#define PEIE    (GPIOE_BASE + 0x38)



/* 初期化処理 */
EXPORT INT usermain(void){
    UINT intsts;

    DI(intsts);
    *(_UW*) PACR &= ~(1<<3); // PA3ポート端子の出力禁止
    *(_UW*) PAIE |= (1<<3);  // PA3ポート端子の入力許可
    *(_UW*) PAFR3 |= (1<<3); // PA3ポート端子をINT3に選択

    SetIntMode(INT3, IM_EDGE | IM_LOW); // 割り込み信号を立下りで検出に設定

    ClearInt(INT3); // 割り込み要因をクリア
    EnableInt(INT3, 5); // 割り込み有効&優先度設定(優先度は0~7)
    EI(intsts); // 割り込み許可

        *(_UW*)PECR |= (1<<3);

    while (1) {
        *(_UB*)(PEDATA) |= (1 << 3);  // LED点灯
        tk_dly_tsk(500);  // 0.5秒待機
        *(_UB*)(PEDATA) &= ~(1 << 3);  // LED消灯
         tk_dly_tsk(500);  // 0.5秒待機
         }
         return 0;
    
}


