#include <tk/tkernel.h>
#include <tm/tmonitor.h>

// GPIOポートA sw31
#define GPIOA_BASE (0x400C0000)
#define PADATA  (GPIOA_BASE + 0x00)
#define PACR    (GPIOA_BASE + 0x04)
#define PAIE    (GPIOA_BASE + 0x38)

// GPIOポートE sw4
#define GPIOE_BASE (0x400C0400)
#define PEDATA  (GPIOE_BASE + 0x00)
#define PECR    (GPIOE_BASE + 0x04)
#define PEIE    (GPIOE_BASE + 0x38)

// GPIOポートH
#define GPIOH_BASE (0x400C0700)
#define PHCR    (GPIOH_BASE + 0x04)
#define PHFR3   (GPIOH_BASE + 0x10)
#define PHIE    (GPIOH_BASE + 0x38)

// MPT2タイマ
#define	MT2_BASE	(0x400C7200UL)
#define	MT2EN   (MT2_BASE + 0x00)
#define	MT2RUN	(MT2_BASE + 0x04)
#define	MT2IGCR	(MT2_BASE + 0x30)
#define	MT2IGOCR	(MT2_BASE + 0x40)
#define	MT2IGRG2	(MT2_BASE + 0x44)
#define	MT2IGRG3	(MT2_BASE + 0x48)
#define	MT2IGRG4	(MT2_BASE + 0x4C)

#define PE_DATA 0x400C0400 //PEデータレジスタのアドレス
#define PE_CR   0x400C0404 //PE出力コントロールレジスタ

// ADC定義
#define ADA_BASE 0x40050000
#define ADACLK  (ADA_BASE + 0x00)
#define ADAMOD0 (ADA_BASE + 0x04)
#define ADAMOD1 (ADA_BASE + 0x08)
#define ADAMOD3 (ADA_BASE + 0x10)
#define ADAMOD4 (ADA_BASE + 0x14)
#define ADAMOD5 (ADA_BASE + 0x18)
#define ADAREG00 (ADA_BASE + 0x34)
#define ADAREG01 (ADA_BASE + 0x38)
#define ADAREG02 (ADA_BASE + 0x3c)
#define ADAREG03 (ADA_BASE + 0x40)

ID sw_flgid; 

void bz_task(INT stacd, void *exinf){
    UW  sw3, sw4;

    // GPIOを入力ポートに設定
    *(_UW*)PACR &= ~(1<<3);
    *(_UW*)PAIE |= (1<<3);

    // MTP2初期設定
    *(_UW*)MT2EN     |= (1<<7) | (1<<0);
    *(_UW*)MT2IGOCR  |= (1<<1) | (0<<5);
    *(_UW*)MT2IGCR   &= ~((1<<6) | (1<<2) | (1<<3) | (1<<0) | (1<<1));  // 0b10110000
    
    // 端子をMTP2に設定
    *(_UW*)PHFR3   |=  (1<<2);
    *(_UW*)PHIE    &=  ~(1<<2);
    *(_UW*)PHCR    |=  (1<<2);
    
    // MTP2 出力波形の設定 
    *(_UW*)MT2IGRG2  = 1;
    *(_UW*)MT2IGRG4  = 18000;
    *(_UW*)MT2IGRG3  = 9000;

    while(1){
    sw3  =  *(_UW*)PADATA & (1<<3); //sw3の読み込み
    if(sw3 == 0){ //sw3を押したら
        tk_set_flg(sw_flgid, (1<<0)); //ビット0のフラグをセット
    }
    sw4  =  *(_UW*)PADATA & (1<<7); //sw4の読み込み
    if(sw4 == 0){ //sw4を押したら
        tk_set_flg(sw_flgid, (1<<1)); //ビット1のフラグをセット
    }  
    tk_dly_tsk(100);
    }
}

void led_task(INT stacd, void *exinf){
    _UW   i;
    UINT d;
	*(_UW*)(PE_CR) |= (1<<2); //PE3出力許可
	*(_UW*)(PE_CR) |= (1<<3); //PE3出力許可

    /* ADC */
    *(_UW*)ADACLK  = 0x01;  // クロック供給
    *(_UW*)ADAMOD1 = 0x80;  // ADC有効化
    tk_dly_tsk(1);   // 3マイクロ秒以上待つ
    *(_UW*)ADAMOD3 = 0x01;  // チャンネルスキャンモード
    *(_UW*)ADAMOD4 = 0x00;  // AINA0の1チャンネルのみ

    UNIT flg = 0;
    while(1){
        tk_wai_flg(sw_flgid, (1<<0)|(1<<1),(TWF_ORW | TWH_BITCLR), &flg, TMO_FEVR);
        if(flg & (1<<0)){
            *(_UW*)(PE_DATA) |= (1<<3); //左PE3'High'出力
            tk_dly_tsk(1000);
            *(_UW*)(PE_DATA) &= ~(1<<3); //左PE3'Low'出力
        }
        else{
            *(_UW*)(PE_DATA) |= (1<<2); //右PE2'High'出力
            tk_dly_tsk(1000);
            *(_UW*)(PE_DATA) &= ~(1<<2); //右PE2'Low'出力
        }
        }   
    }
    
EXPORT int usermain(void){
    T_CTSK ctsk;
    ID tskid1;

    ID sw_flgid;
    T_CFLG cflg;
    cflg.flgatr  = TA_WMUL | TA_TFIFO; //複数のタスク待ちを許可 | 並び順は先着順
    cflg.iflgptn = 0;
    sw_flgid = tk_cre_flg(&cflg);
    
    ctsk.tskatr  = TA_HLNG | TA_RNG3;
    ctsk.task    = (FP)bz_task;
    ctsk.itskpri = 10;
    ctsk.stksz   = 1024;
    tskid1       = tk_cre_tsk( &ctsk );

    ctsk.task    = (FP)led_task;
    ctsk.itskpri = 10;
    led_tskid    = tk_cre_tsk( &ctsk );
    
    tk_sta_tsk(tskid1, 0);

    tk_slp_tsk(TMO_FEVR);

    
    

    return 0;
}
