#include <tk/tkernel.h>
#include <tm/tmonitor.h>
#include <tk/device.h> // デバイスの各種宣言、定義

ER set_speed(ID dd, INT speed) {
    UW data;
    SZ asz;
    ER err;

    if (speed < -500 || speed > 500) return E_PAR;
    data = 14000 - (1500 - speed);
    err = tk_swri_dev(dd, PMD_DATA_PULSE0, &data, 1, &asz);
    return err;
}

ER init_pmd(ID dd) {
    UW data;
    SZ asz;
    ER err;

    data = 14000;        // 矩形波の周期 14,000μ秒
    err = tk_swri_dev(dd, PMD_DATA_PERIOD, &data, 1, &asz);
    if (err != E_OK) return err;

    data = 14000 - 1500; // 走行モータの初期値(停止)
    err = tk_swri_dev(dd, PMD_DATA_PULSE0, &data, 1, &asz);
    if (err != E_OK) return err;

    data = 1500;         // ステアリングの初期値(直進)
    err = tk_swri_dev(dd, PMD_DATA_PULSE1, &data, 1, &asz);
    if (err != E_OK) return err;

    data = 1;            // 出力開始
    err = tk_swri_dev(dd, PMD_DATA_EN, &data, 1, &asz);
    if (err != E_OK) return err;

    return E_OK;
}

LOCAL void drv_task(void *exinf) {
    /* 変数の宣言 */
    ID pmd_dd;
    ER err;
    INT speed;

    pmd_dd = tk_opn_dev("pmda", TD_UPDATE);   // PMDドライバのオープン
    err = init_pmd(pmd_dd);                   // PMDデバイスの初期設定
    if (err != E_OK) {
        tm_printf("PMD initialization failed: %d\n", err);
        tk_cls_dev(pmd_dd, 0);
        return;
    }

    for (speed = 50; speed < 62; speed++) {
        set_speed(pmd_dd, speed);             // 速度制御
        tm_printf("SPEED = %d\n", speed);
        tk_dly_tsk(1000);                     // 1秒待機
    }

    tk_cls_dev(pmd_dd, 0);                    // デバイスのクローズ
}

EXPORT INT usermain(void) {
    T_CTSK drv_ctsk;          /* 走行タスク生成情報 */
    ER err;
    ID drv_tskid;

    /* 走行制御制御タスク生成・実行 */
    drv_ctsk.tskatr = TA_HLNG | TA_RNG3;  /* タスク属性 */
    drv_ctsk.task = (FP)drv_task;         /* 走行タスクの処理関数 */
    drv_ctsk.itskpri = 10;                /* タスク起動時優先度 */
    drv_ctsk.stksz = 1024;                /* スタックサイズ */

    drv_tskid = tk_cre_tsk(&drv_ctsk);    /* 走行タスクの生成 */
    tk_sta_tsk(drv_tskid, 0);             /* 走行タスクの起動 */

    tk_slp_tsk(TMO_FEVR);                 /* 初期タスクを休止状態へ */
                                          /* TMO_FEVRは無限待ち */
    return 0;
}

