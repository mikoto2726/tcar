#include <tk/tkernel.h>
#include <tm/tmonitor.h>

// GPIOポートA sw3
#define GPIOA_BASE (0x400C0000)
#define PADATA  (GPIOA_BASE + 0x00)
#define PACR    (GPIOA_BASE + 0x04)
#define PAIE    (GPIOA_BASE + 0x38)

// GPIOポートE sw4
#define GPIOE_BASE (0x400C0400)
#define PEDATA  (GPIOE_BASE + 0x00)
#define PECR    (GPIOE_BASE + 0x04)
#define PEIE    (GPIOE_BASE + 0x38)

#define PE_DATA 0x400C0400 //PEデータレジスタのアドレス
#define PE_CR   0x400C0404 //PE出力コントロールレジスタ

ID sw_flgid; // フラグIDをグローバル変数として宣言

void bz_task(INT stacd, void *exinf){
    UW sw3, sw4;

    // GPIOを入力ポートに設定
    *(_UW*)PACR &= ~(1<<3);
    *(_UW*)PAIE |= (1<<3);

    while(1){
        sw3 = *(_UW*)PADATA & (1<<3); // sw3の読み込み
        if(sw3 == 0){ // sw3を押したら
            tk_set_flg(sw_flgid, (1<<0)); // ビット0のフラグをセット
        }
        sw4 = *(_UW*)PEDATA & (1<<7); // sw4の読み込み
        if(sw4 == 0){ // sw4を押したら
            tk_set_flg(sw_flgid, (1<<1)); // ビット1のフラグをセット
        }  
        tk_dly_tsk(100);
    }
}

void led_task(INT stacd, void *exinf){
    *(_UW*)(PE_CR) |= (1<<2); // PE2出力許可
    *(_UW*)(PE_CR) |= (1<<3); // PE3出力許可

    UINT flg = 0;
    while(1){
        tk_wai_flg(sw_flgid, (1<<0)|(1<<1),(TWF_ORW | TWF_BITCLR), &flg, TMO_FEVR);
        if(flg & (1<<0)){
            *(_UW*)(PE_DATA) |= (1<<3); // 左PE3'High'出力
            tk_dly_tsk(1000);
            *(_UW*)(PE_DATA) &= ~(1<<3); // 左PE3'Low'出力
        }
        else if(flg & (1<<1)){
            *(_UW*)(PE_DATA) |= (1<<2); // 右PE2'High'出力
            tk_dly_tsk(1000);
            *(_UW*)(PE_DATA) &= ~(1<<2); // 右PE2'Low'出力
        }
    }
}
    
EXPORT int usermain(void){
    T_CTSK ctsk;
    ID tskid1, tskid2;

    // フラグIDの生成
    T_CFLG cflg;
    cflg.flgatr  = TA_WMUL | TA_TFIFO; // 複数のタスク待ちを許可 | 並び順は先着順
    cflg.iflgptn = 0;
    sw_flgid = tk_cre_flg(&cflg);

    // bz_task のタスク生成
    ctsk.tskatr  = TA_HLNG | TA_RNG3;
    ctsk.task    = (FP)bz_task;
    ctsk.itskpri = 10;
    ctsk.stksz   = 1024;
    tskid1       = tk_cre_tsk(&ctsk);

    // led_task のタスク生成
    ctsk.task    = (FP)led_task;
    ctsk.itskpri = 10;
    tskid2       = tk_cre_tsk(&ctsk);

    // タスクの開始
    tk_sta_tsk(tskid1, 0);
    tk_sta_tsk(tskid2, 0);

    tk_slp_tsk(TMO_FEVR);
    
    return 0;
}
