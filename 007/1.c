 #include <tk/tkernel.h>  /* T-Kernel ヘッダ */
#include <tm/tmonitor.h> /* tm_printf() など */

/* GPIO(Port-E) */
#define PEDATA      0x400C0400     // Data Register
#define PECR        0x400C0404     // Output Control register
#define PEIE        0x400C0438     // Input Control register

/* GPIOポートA */
#define GPIOA_BASE  0x400C0000
#define PADATA      (GPIOA_BASE + 0x00)
#define PACR        (GPIOA_BASE + 0x04)
#define PAIE        (GPIOA_BASE + 0x38)

ID mbfid; // メッセージバッファID

void init_ports() {
    // Port Aの初期化
    *(_UW*)PACR |= (1 << 3);  // PA3を入力モードに設定
    *(_UW*)PAIE |= (1 << 3);  // PA3の入力を有効化

    // Port Eの初期化
    *(_UW*)PECR |= (1 << 3) | (1 << 2);  // PE3とPE2を出力モードに設定
    *(_UW*)PEIE |= (1 << 3) | (1 << 2);  // PE3とPE2の出力を有効化
}

void create_message_buffer() {
    T_CMBF cmbf; /* メッセージバッファ生成 */
    cmbf.mbfatr = TA_TFIFO; /* メッセージバッファ属性 */
    cmbf.bufsz = sizeof(INT) * 10; /* メッセージバッファのサイズ */
    cmbf.maxmsz = sizeof(INT); /* メッセージの最大長 */
    mbfid = tk_cre_mbf(&cmbf); /* メッセージバッファ生成 */
}

void sw_monitor_task(INT stacd, void *exinf) {
    INT msg;
    while(1) {
        int sw3 = *(_UW*)PADATA & (1 << 3); // SW3を読み込む
        if(sw3 == 0) {
            msg = 3;
            tk_snd_mbf(mbfid, &msg, sizeof(msg), TMO_FEVR);
        }

        int sw4 = *(_UW*)PEDATA & (1 << 7); // SW4を読み込む
        if(sw4 == 0) {
            msg = 4;
            tk_snd_mbf(mbfid, &msg, sizeof(msg), TMO_FEVR);
        }

        tk_dly_tsk(100); // デバウンス処理のための遅延
    }
}

void led_control_task(INT stacd, void *exinf) {
    INT msg;
    while(1) {
        tk_rcv_mbf(mbfid, &msg, TMO_FEVR);
        if(msg == 3) {
            *(_UB*)(PEDATA) |= (1 << 3); // LED(PE3)をHighに設定
            tk_dly_tsk(1000); // 1秒間待つ
            *(_UB*)(PEDATA) &= ~(1 << 3); // LED(PE3)をLowに設定
        } else if(msg == 4) {
            *(_UB*)(PEDATA) |= (1 << 2); // LED(PE2)をHighに設定
            tk_dly_tsk(1000); // 1秒間待つ
            *(_UB*)(PEDATA) &= ~(1 << 2); // LED(PE2)をLowに設定
        }
    }
}

int usermain(void) {
    init_ports(); // ポートの初期化
    create_message_buffer(); // メッセージバッファ生成

    // タスク生成と起動
    T_CTSK sw_monitor_ctsk;
    sw_monitor_ctsk.tskatr = TA_HLNG | TA_RNG3;
    sw_monitor_ctsk.task = (FP)sw_monitor_task;
    sw_monitor_ctsk.itskpri = 10;
    sw_monitor_ctsk.stksz = 1024;
    sw_monitor_ctsk.bufptr = NULL;
    sw_monitor_ctsk.exinf = NULL;
    ID sw_monitor_tid = tk_cre_tsk(&sw_monitor_ctsk);

    T_CTSK led_control_ctsk;
    led_control_ctsk.tskatr = TA_HLNG | TA_RNG3;
    led_control_ctsk.task = (FP)led_control_task;
    led_control_ctsk.itskpri = 10;
    led_control_ctsk.stksz = 1024;
    led_control_ctsk.bufptr = NULL;
    led_control_ctsk.exinf = NULL;
    ID led_control_tid = tk_cre_tsk(&led_control_ctsk);

    tk_sta_tsk(sw_monitor_tid, 0); // SW監視タスク開始
    tk_sta_tsk(led_control_tid, 0); // LED制御タスク開始

    return 0;
}
