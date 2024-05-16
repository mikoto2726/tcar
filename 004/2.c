#include <tk/tkernel.h>  /* T-Kernel ヘッダ */
#include <tm/tmonitor.h> /* tm_printf() など */

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

/* LED制御タスクの処理関数 */
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

        tk_dly_tsk(500); // 500 ミリ秒待つ
    }
}

EXPORT INT usermain(void)
{
    T_CTSK led_ctsk;
    ID led_tskid;

    /* タスク生成情報 */
    led_ctsk.tskatr = TA_HLNG | TA_RNG3; /* タスク属性 */
    led_ctsk.task = led_task;
    led_ctsk.itskpri = 10; /* タスク起動時優先度 */
    led_ctsk.stksz = 1024; /* スタックサイズ */

    led_tskid = tk_cre_tsk(&led_ctsk);
    tk_sta_tsk(led_tskid, 0); /* LED制御タスクの生成と起動 */

    tk_slp_tsk(TMO_FEVR); /* 初期タスクを休止状態へ（無限待ち） */
    return 0;
}
