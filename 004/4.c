 #include <tk/tkernel.h>  /* T-Kernel ヘッダ */
#include <tm/tmonitor.h> /* tm_printf() など */

// GPIOポートA
#define GPIOA_BASE (0x400C0000)
#define PADATA  (GPIOA_BASE + 0x00)
#define PACR    (GPIOA_BASE + 0x04)
#define PAIE    (GPIOA_BASE + 0x38)

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

/* GPIO(Port-E) */
#define PEDATA      0x400C0400     // Data Register
#define PECR        0x400C0404     // Output Control register
#define PEIE        0x400C0438     // Input Control register

/* ADC定義 */
#define ADA_BASE    0x40050000
#define ADACLK      (ADA_BASE + 0x00)
#define ADAMOD0     (ADA_BASE + 0x04)
#define ADAMOD1     (ADA_BASE + 0x08)
#define ADAMOD3     (ADA_BASE + 0x10)
#define ADAMOD4     (ADA_BASE + 0x14)
#define ADAMOD5     (ADA_BASE + 0x18)
#define ADAREG00    (ADA_BASE + 0x34)
#define ADAREG01    (ADA_BASE + 0x38)
#define ADAREG02    (ADA_BASE + 0x3c)
#define ADAREG03    (ADA_BASE + 0x40)

/* ブザー制御タスクの処理関数 */
void bz_task(INT stacd, void *exinf)
{
    UW sw3_prev = 0;  // 最初のSW3の状態を保持する変数

      // MTP2初期設定
    *(_UW*)MT2EN     |=  (1 << 7) | (1 << 0);
    *(_UW*)MT2IGOCR  |= (1 << 1);
    *(_UW*)MT2IGOCR  &= ~(1 << 5);
    *(_UW*)MT2IGCR   &= ~(1 << 6);
    *(_UW*)MT2IGCR   &= 0xF0;

    // GPIOを入力ポートに設定 初期化 端子
    *(_UW*)PACR &= ~(1 << 3);
    *(_UW*)PAIE |= (1 << 3);
      // 端子をMTP2に設定
    *(_UW*)PHFR3   |=  (1 << 2);
    *(_UW*)PHIE    &= ~(1 << 2);
    *(_UW*)PHCR   |=  (1 << 2);
  

    

      // MTP2 出力波形の設定 
    *(_UW*)MT2IGRG2  = 1;
    *(_UW*)MT2IGRG4  = 18000;
    *(_UW*)MT2IGRG3  = 9000;

    while(1) {
        UW sw3 = *(_UW*)PADATA & (1 << 3);  // SW3の状態を読み取る
        if (sw3 != sw3_prev) {  // SW3の状態が変化したとき
            sw3_prev = sw3;  // 前回の状態を更新
            if (sw3 == 0) {  // SW3が押されたとき
                // MPT2からブザーに波形を出力するコード
                *(_UW*)MT2RUN |= (1 << 2);  // MPT2の出力2番をONにする
                *(_UW*)MT2RUN |= (1 << 0);  // MPT2の出力0番をONにする
                tk_dly_tsk(2000);  // 2秒間待機
                *(_UW*)MT2RUN &= ~(1 << 2);  // MPT2の出力2番をOFFにする
                *(_UW*)MT2RUN &= ~(1 << 0);  // MPT2の出力0番をOFFにする
            }
        }
        tk_dly_tsk(100);  // 一定時間待機
    }
}

void led_task(INT stacd, void *exinf)
{
    UW d;

    /* GPIO初期化 */
    *(_UW*)PEIE &= ~(1<<2);
    *(_UW*)PECR |= (1<<2);

    /* ADC初期化 */
    *(_UW*)ADACLK  = 0x01; // クロック供給
    *(_UW*)ADAMOD1 = 0x80; // ADC有効化
    tk_dly_tsk(1); // 3マイクロ秒以上待つ
    *(_UW*)ADAMOD3 = 0x01; // チャンネルスキャンモード
    *(_UW*)ADAMOD4 = 0x00; // AINA0の1チャンネルのみ

    while(1) {
        /* A/D変換を行う */
        *(_UW*)ADAMOD0 = 0x01; // 変換開始
        while ((*(_UW*)ADAMOD5 & 0x02) == 0); // 変換完了待ち
        d = *(_UW*)ADAREG00 & 0x0FFF; // AINA0の変換結果 12ビット分のみを読み出す

        /* 照度センサーの値と変換結果を表示 */
        tm_printf((UB*)"sensor value: %d,  result: %d\n", 900, d);


        /* 照度センサーの値に応じてLED制御 */
        if (d < 900) {
            /* 照度が低い場合、LEDを点灯 */
            *(_UW*)PEDATA |= (1<<2); // 左ウィンカーLEDを点灯
        } else {
            /* 照度が高い場合、LEDを消灯 */
            *(_UW*)PEDATA &= ~(1<<2); // 左ウィンカーLEDを消灯
        }

        for(int i=0;i<100000;i++); // 課題4-1

    }
}


EXPORT INT usermain(void)
{
    T_CTSK led_ctsk, bz_ctsk;
    ID led_tskid, bz_tskid;

    /* LED制御タスク生成情報 */
    led_ctsk.tskatr = TA_HLNG | TA_RNG3; /* タスク属性 */
    led_ctsk.task = led_task;
    led_ctsk.itskpri = 10; /* タスク起動時優先度 */
    led_ctsk.stksz = 1024; /* スタックサイズ */

    /* ブザー制御タスク生成情報 */
    bz_ctsk.tskatr = TA_HLNG | TA_RNG3; /* タスク属性 */
    bz_ctsk.task = bz_task;
    bz_ctsk.itskpri = 11; /* タスク起動時優先度 */
    bz_ctsk.stksz = 1024; /* スタックサイズ */

    /* LED制御タスクの生成と起動 */
    led_tskid = tk_cre_tsk(&led_ctsk);
    tk_sta_tsk(led_tskid, 0);

    /* ブザー制御タスクの生成と起動 */
    bz_tskid = tk_cre_tsk(&bz_ctsk);
    tk_sta_tsk(bz_tskid, 0);

    tk_slp_tsk(TMO_FEVR); /* 初期タスクを休止状態へ（無限待ち） */
    return 0;
}
