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

ID led_tskid; 

void sw_task(INT stacd, void *exinf){
     UW sw3;

    // GPIOを入力ポートに設定
    *(_UW*)PACR &= ~(1<<3); //sw3 左
    *(_UW*)PAIE |= (1<<3);

    while(1) {
        sw3  =  *(_UW*)PADATA & (1<<3); //sw3を読み込む
        if(sw3 == 0){
        	tk_wup_tsk(led_tskid);//LED制御タスクを起床
    	}
        tk_dly_tsk(100);//0.1秒待つ
    }
}


void led_task(INT stacd, void *exinf){
    _UW   i;
    UINT d;
	*(_UW*)(PE_CR) |= (1<<2); //PE2出力許可
	*(_UW*)(PE_CR) |= (1<<3); //PE3出力許可

    while(1){
        tk_slp_tsk(TMO_FEVR);  
        *(_UW*)(PE_DATA) |= (1<<3); //PE3'High'出力
        tk_dly_tsk(1000); // 500 ミリ秒待つ
        *(_UW*)(PE_DATA) &= ~(1<<3); //PE3'Low'出力
        }   
    }
    
EXPORT int usermain(void){
    T_CTSK ctsk;
    ID tskid1;

    ctsk.tskatr  = TA_HLNG | TA_RNG3;
    ctsk.task    = (FP)sw_task;
    ctsk.itskpri = 10;
    ctsk.stksz   = 1024;
    tskid1       = tk_cre_tsk( &ctsk );

    ctsk.task    = (FP)led_task;
    ctsk.itskpri = 10;
    led_tskid    = tk_cre_tsk( &ctsk );
    
    tk_sta_tsk(tskid1, 0);
    tk_sta_tsk(led_tskid, 0);

    tk_slp_tsk(TMO_FEVR);
    return 0;

}
