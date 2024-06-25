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
    


void led_tskid(INT stack, void *exinf) {
    *(_UW*)(PECR) |= (1<<3); //PE3出力許可

    while(1){
        *(_UW*)(PEDATA) |= (1<<3); //PE3'High'出力
        tk_dly_tsk(500); // 500 ミリ秒待つ
        *(_UW*)(PEDATA) &= ~(1<<3); //PE3'Low'出力
        tk_dly_tsk(500); // 500 ミリ秒待つ
    } 
}

void inthdr(UINT intno){
    ClearInt(INT3);
    tk_wup_tsk(led_tskid);
    return;
}

EXPORT int usermain(void) {
    UINT intsts;
    
    DI(intsts);//全ての割り込みを禁止

    T_DINT dint; //割り込みハンドラ定義情報
    //割り込みハンドラ定義
    dint.intatr = TA_HLNG; //割り込みハンドラ属性
    dint.inthdr = inthdr; //割り込みハンドラの実行アドレス(割り込みハンドラの関数名)
    tk_def_int(INT3, &dint); //割り込みハンドラの定義

    *(_UW*)PACR &= ~(1 << 3);  //PA3ポート端子の出力禁止
    *(_UW*)PAIE |= (1 << 3);   //PA3ポート端子の入力許可
    *(_UW*)PAFR3 &= ~(1 << 3); //PA3ポート端子をINT3に選択
    SetIntMode(INT3, IM_EDGE | IM_LOW); // 割り込み信号を立下りで検出に設定

    ClearInt(INT3); //割り込み要因クリア
    EnableInt(INT3, 5); //割り込み有効&優先度設定(優先度は0~7)

    EI(intsts); //DIによる割り込み禁止の解除

    

    return 0;
}
