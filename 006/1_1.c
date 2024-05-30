#include <tk/tkernel.h>
#include <tk/device.h>
#include <tm/tmonitor.h>  // tm_printf() など

// タスクID
#define TASK_A_ID 1
#define TASK_B_ID 2
#define TASK_C_ID 3

// タスクのスタックサイズ
#define STACK_SIZE 1024

// タスクの優先度
#define TASK_PRIORITY 10

// GPIO(Port-E) レジスタアドレス
#define PEDATA 0x400C0400     // Data Register
#define PECR 0x400C0404       // Output Control register
#define PEIE 0x400C0438       // Input Control register

// LED制御関数
void led_control(int state) {
    if (state) {
        *(_UW*)PEDATA |= (1 << 2); // LED点灯
    } else {
        *(_UW*)PEDATA &= ~(1 << 2); // LED消灯
    }
    tm_printf("LED state: %d\n", state); // デバッグ用出力
}

// タスクA (0.5秒毎にLEDの点灯・消灯を繰り返す)
void taskA(INT stacd, void *exinf) {
    while (1) {
        led_control(1); // LED点灯
        tk_dly_tsk(500); // 0.5秒待機
        led_control(0); // LED消灯
        tk_dly_tsk(500); // 0.5秒待機
    }
}

// タスクB (0.1秒毎にLEDの点灯・消灯を繰り返す)
void taskB(INT stacd, void *exinf) {
    while (1) {
        led_control(1); // LED点灯
        tk_dly_tsk(100); // 0.1秒待機
        led_control(0); // LED消灯
        tk_dly_tsk(100); // 0.1秒待機
    }
}

// タスクC (1秒毎にLEDの点灯・消灯を繰り返す)
void taskC(INT stacd, void *exinf) {
    while (1) {
        led_control(1); // LED点灯
        tk_dly_tsk(1000); // 1秒待機
        led_control(0); // LED消灯
        tk_dly_tsk(1000); // 1秒待機
    }
}

// メイン関数
EXPORT INT usermain(void) {
    // GPIO初期化
    *(_UW*)PEIE &= ~(1 << 2);
    *(_UW*)PECR |= (1 << 2);

    tm_printf("GPIO initialized\n");

    // タスクAの作成
    T_CTSK ctskA = {0};
    ctskA.tskatr = TA_HLNG | TA_RNG3;
    ctskA.stksz = STACK_SIZE;
    ctskA.itskpri = TASK_PRIORITY;
    ctskA.task = taskA;
    ID tskA_id = tk_cre_tsk(&ctskA);
    if (tskA_id < E_OK) {
        tm_printf("Failed to create task A: %d\n", tskA_id);
        return tskA_id;
    }
    tk_sta_tsk(tskA_id, 0);

    // タスクBの作成
    T_CTSK ctskB = {0};
    ctskB.tskatr = TA_HLNG | TA_RNG3;
    ctskB.stksz = STACK_SIZE;
    ctskB.itskpri = TASK_PRIORITY;
    ctskB.task = taskB;
    ID tskB_id = tk_cre_tsk(&ctskB);
    if (tskB_id < E_OK) {
        tm_printf("Failed to create task B: %d\n", tskB_id);
        return tskB_id;
    }
    tk_sta_tsk(tskB_id, 0);

    // タスクCの作成
    T_CTSK ctskC = {0};
    ctskC.tskatr = TA_HLNG | TA_RNG3;
    ctskC.stksz = STACK_SIZE;
    ctskC.itskpri = TASK_PRIORITY;
    ctskC.task = taskC;
    ID tskC_id = tk_cre_tsk(&ctskC);
    if (tskC_id < E_OK) {
        tm_printf("Failed to create task C: %d\n", tskC_id);
        return tskC_id;
    }
    tk_sta_tsk(tskC_id, 0);

    tm_printf("Tasks started\n");

    // メインタスクを無限待機状態にする
    tk_slp_tsk(TMO_FEVR);
    return 0;
}
