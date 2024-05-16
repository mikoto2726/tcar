#include <tk/tkernel.h>  /* T-Kernel ヘッダ */
#include <tm/tmonitor.h> /* tm_printf() など */

/* GPIO(Port-E) */
#define PEDATA      0x400C0400     // Data Register
#define PECR        0x400C0404     // Output Control register
#define PEIE        0x400C0438     // Input Control register

// GPIOポートA
#define GPIOA_BASE (0x400C0000)
#define PADATA  (GPIOA_BASE + 0x00)
#define PACR    (GPIOA_BASE + 0x04)
#define PAIE    (GPIOA_BASE + 0x38)

/* グローバル変数 */
ID led_tskid;


/* LED制御タスクの処理関数 */
void led_task(INT stacd, void *exinf)
{
    *(_UW*)PEIE &= ~(1<<2);
    *(_UW*)PECR |= (1<<2);

    while (1) {
        tk_slp_tsk(TMO_FEVR);  // タスクのスリープ
        *(_UB*)(PEDATA) |= (1 << 3);  // LED点灯
        tk_dly_tsk(1000);  // 1秒待機
        *(_UB*)(PEDATA) &= ~(1 << 3);  // LED消灯
    }
}

/* SW監視タスクの処理関数 */
void sw_task(INT stacd, void *exinf)
{
    UW sw3;
        // GPIOを入力ポートに設定 初期化 端子
    *(_UW*)PACR &= ~(1 << 3);
    *(_UW*)PAIE |= (1 << 3);



    while (1) {
        sw3 = *(_UW*)PADATA & (1 << 3);  // SW3の状態を読み込む
        if (sw3 == 0) {  // SW3が押されている場合
            tk_wup_tsk(led_tskid);  // LEDタスクを起床
        }
        tk_dly_tsk(100);  // 0.1秒待機
    }
}

EXPORT INT usermain(void)
{
    T_CTSK led_ctsk, sw_ctsk;
    ID sw_tskid;

    /* LED制御タスク生成情報 */
    led_ctsk.tskatr = TA_HLNG | TA_RNG3; /* タスク属性 */
    led_ctsk.task = led_task;
    led_ctsk.itskpri = 10; /* タスク起動時優先度 */
    led_ctsk.stksz = 1024; /* スタックサイズ */

    /* SW監視タスク生成情報 */
    sw_ctsk.tskatr = TA_HLNG | TA_RNG3; /* タスク属性 */
    sw_ctsk.task = sw_task;
    sw_ctsk.itskpri = 10; /* タスク起動時優先度 */
    sw_ctsk.stksz = 1024; /* スタックサイズ */

    led_tskid = tk_cre_tsk(&led_ctsk);
    sw_tskid = tk_cre_tsk(&sw_ctsk);

    if (led_tskid < 0 || sw_tskid < 0) {
        tm_printf((UB*)"Error creating tasks\n");
        return -1;
    }

    if (tk_sta_tsk(led_tskid, 0) != E_OK || tk_sta_tsk(sw_tskid, 0) != E_OK) {
        tm_printf((UB*)"Error starting tasks\n");
        return -1;
    }

    tk_slp_tsk(TMO_FEVR); /* 初期タスクを休止状態へ（無限待ち） */
    return 0;
}
