#include <tk/tkernel.h>
#include <tm/tmonitor.h>
#include <tk/device.h>

// PMDデバイスの固有データ定義
#define PMD_DATA_EN       0
#define PMD_DATA_PERIOD   1
#define PMD_DATA_PULSEO   2
#define PMD_DATA_PULSE1   3
#define PMD_DATA_PULSE2   4

#define SENSOR_MAX 4
#define SNS_TH 10

const UB sensor_addr[SENSOR_MAX] = {0x38, 0x39, 0x3c, 0x3D};

typedef struct {
    UH red;
    UH green;
    UH blue;
    UH clear;
} T_COLOR_DATA;

T_COLOR_DATA cdata[SENSOR_MAX];
INT diff, steer;

// プロトタイプ宣言
ER set_steer(ID dd, INT steer);
ER set_speed(ID dd, INT speed);
ER init_pmd(ID dd);
ER read_sensor(ID dd, T_COLOR_DATA cdata[]);

ER set_steer(ID dd, INT steer) {
    UW data;
    SZ asz;
    ER err;

    if (steer < -500 || steer > 500) {
        return E_PAR;
    }

    data = steer + 1500; // -500 ~ +500 を 1000 ~ 2000 の範囲にする
    err = tk_swri_dev(dd, PMD_DATA_PULSE1, &data, 1, &asz);

    return err;
}

ER set_speed(ID dd, INT speed) {
    UW data;
    SZ asz;
    ER err;

    if (speed < -500 || speed > 500) return E_PAR;
    data = 14000 - (1500 - speed);
    err = tk_swri_dev(dd, PMD_DATA_PULSEO, &data, 1, &asz);
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
    err = tk_swri_dev(dd, PMD_DATA_PULSEO, &data, 1, &asz);
    if (err != E_OK) return err;

    data = 1500;         // ステアリングの初期値(直進)
    err = tk_swri_dev(dd, PMD_DATA_PULSE1, &data, 1, &asz);
    if (err != E_OK) return err;

    data = 1;            // 出力開始
    err = tk_swri_dev(dd, PMD_DATA_EN, &data, 1, &asz);
    if (err != E_OK) return err;

    return E_OK;
}

ER read_sensor(ID dd, T_COLOR_DATA cdata[]) {
    UB data[2];
    SZ asz;
    ER err;
    INT i;

    for (i = 0; i < SENSOR_MAX; i++){
        data[0] = 0x40;
        data[1] = 0x80;
        err = tk_swri_dev(dd, sensor_addr[i], &data, 2, &asz);

        data[0] = 0x40;
        data[1] = 0x00;
        err = tk_swri_dev(dd, sensor_addr[i], &data, 2, &asz);

        data[0] = 0x42;
        data[1] = 0x10;
        err = tk_swri_dev(dd, sensor_addr[i], &data, 2, &asz);
    }

    for (i = 0; i < SENSOR_MAX; i++){
        while(1) {
            T_I2C_EXEC exec;
            UB snd_data;
            UB rcv_data;

            exec.sadr = sensor_addr[i];
            snd_data = 0x42;
            exec.snd_size = 1;
            exec.snd_data = &snd_data;
            exec.rcv_size = 1;
            exec.rcv_data = &rcv_data;
            err = tk_swri_dev(dd, TDN_I2C_EXEC, &exec, sizeof(T_I2C_EXEC), &asz);

            if(rcv_data & 0x80){
                UH sens_data[4];

                exec.sadr = sensor_addr[i];
                snd_data = 0x50;
                exec.snd_size = 1;
                exec.snd_data = &snd_data;
                exec.rcv_size = sizeof(sens_data);
                exec.rcv_data = (UB*)(sens_data);
                err = tk_swri_dev(dd, TDN_I2C_EXEC, &exec, sizeof(T_I2C_EXEC), &asz);

                cdata[i].red   = sens_data[0];
                cdata[i].green = sens_data[1];
                cdata[i].blue  = sens_data[2];
                cdata[i].clear = sens_data[3];
                break;
            }
        }
    }

    return err;
}

LOCAL void drv_task(void *exinf) {
    ID sns_dd, pmd_dd;
    ER err;

    sns_dd = tk_opn_dev("iicb", TD_UPDATE);   // I2Cドライバのオープン
    if (sns_dd < 0) {
        tm_printf("Failed to open I2C driver\n");
        return;
    }

    pmd_dd = tk_opn_dev("pmda", TD_UPDATE);   // PMDドライバのオープン
    if (pmd_dd < 0) {
        tm_printf("Failed to open PMD driver\n");
        tk_cls_dev(sns_dd, 0);
        return;
    }

    err = init_pmd(pmd_dd);                   // PMDデバイスの初期設定
    if (err < 0) {
        tm_printf("PMD initialization failed: %d\n", err);
        tk_cls_dev(sns_dd, 0);
        tk_cls_dev(pmd_dd, 0);
        return;
    }

    set_speed(pmd_dd, 50); // 初期速度を設定

    while (1) {
        err = read_sensor(sns_dd, cdata); // カラーセンサの読込み
        if (err < 0) {
            tm_printf("Sensor read failed: %d\n", err);
            continue;
        }

        diff = (cdata[2].clear + cdata[3].clear) - (cdata[0].clear + cdata[1].clear);
        if (diff > SNS_TH) {              // 左が大きい
            steer = 500;                  // 左折する
        } else if (diff < -SNS_TH) {      // 右が大きい
            steer = -500;                 // 右折する
        } else {                          // 左右同じ
            steer = 0;                    // 直進する
        }
        err = set_steer(pmd_dd, steer);   // ステアリング制御
        if (err < 0) {
            tm_printf("Failed to set steer: %d\n", err);
        } else {
            tm_printf("Steer set to %d\n", steer);
        }

        tk_dly_tsk(100);                  // 100msの待機
    }

    tk_cls_dev(sns_dd, 0);                // デバイスのクローズ
    tk_cls_dev(pmd_dd, 0);                // デバイスのクローズ
}

EXPORT INT usermain(void) {
    T_CTSK drv_ctsk;          /* 走行タスク生成情報 */
    ID drv_tskid;

    drv_ctsk.tskatr = TA_HLNG | TA_RNG3;  /* タスク属性 */
    drv_ctsk.task = (FP)drv_task;         /* 走行タスクの処理関数 */
    drv_ctsk.itskpri = 10;                /* タスク起動時優先度 */
    drv_ctsk.stksz = 1024;                /* スタックサイズ */

    drv_tskid = tk_cre_tsk(&drv_ctsk);    /* 走行タスクの生成 */
    if (drv_tskid < 0) {
        tm_printf("Failed to create task\n");
        return drv_tskid;
    }
    tk_sta_tsk(drv_tskid, 0);             /* 走行タスクの起動 */

    tk_slp_tsk(TMO_FEVR);                 /* 初期タスクを休止状態へ */
                                          /* TMO_FEVRは無限待ち */
    return E_OK;
}
