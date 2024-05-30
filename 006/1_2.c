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

// セマフォの属性
#define SEMAPHORE_ATTR (TA_TFIFO)

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

// タスクA
void taskA(INT stacd, void *exinf) {
    ID semid = *(ID*)exinf;
    while (1) {
        tk_wai_sem(semid, 1, TMO_FEVR);
        for (int i = 0; i < 10; i++) { // 10秒間 (20回0.5秒)
            led_control(1); // LED点灯
            tk_dly_tsk(500); // 0.5秒待機
            led_control(0); // LED消灯
            tk_dly_tsk(500); // 0.5秒待機
        }
        tk_sig_sem(semid, 1);
    }
}

// タスクB
void taskB(INT stacd, void *exinf) {
    ID semid = *(ID*)exinf;
    while (1) {
        tk_wai_sem(semid, 1, TMO_FEVR);
        for (int i = 0; i < 10; i++) { // 10秒間 (20回0.5秒)
            led_control(1); // LED点灯
            tk_dly_tsk(500); // 0.5秒待機
            led_control(0); // LED消灯
            tk_dly_tsk(500); // 0.5秒待機
        }
        tk_sig_sem(semid, 1);
    }
}

// タスクC
void taskC(INT stacd, void *exinf) {
    ID semid = *(ID*)exinf;
    while (1) {
        tk_wai_sem(semid, 1, TMO_FEVR);
        for (int i = 0; i < 10; i++) { // 10秒間 (20回0.5秒)
            led_control(1); // LED点灯
            tk_dly_tsk(500); // 0.5秒待機
            led_control(0); // LED消灯
            tk_dly_tsk(500); // 0.5秒待機
        }
        tk_sig_sem(semid, 1);
    }
}

// メイン関数
EXPORT INT usermain(void) {
    // GPIO初期化は事前に行われているとする
    tm_printf("GPIO initialized\n");

    // セマフォの作成
    T_CSEM csem = {0};
    csem.sematr = SEMAPHORE_ATTR;
    csem.isemcnt = 1;
    csem.maxsem = 1;
    ID semid = tk_cre_sem(&csem);
    if (semid < E_OK) {
        // セマフォの作成に失敗
        tm_printf("Failed to create semaphore: %d\n", semid);
        return semid;
    }

    // タスクAの作成
    T_CTSK ctskA = {0};
    ctskA.tskatr = TA_HLNG | TA_RNG3;
    ctskA.stksz = STACK_SIZE;
    ctskA.itskpri = TASK_PRIORITY;
    ctskA.task = taskA;
    ctskA.exinf = &semid; // セマフォIDをタスクに渡す
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
    ctskB.exinf = &semid; // セマフォIDをタスクに渡す
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
    ctskC.exinf = &semid; // セマフォIDをタスクに渡す
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
