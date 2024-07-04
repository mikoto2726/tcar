#include <tk/tkernel.h>
#include <tm/tmonitor.h>
#include <tk/device.h> // デバイスの各種宣言、定義

#define SENSOR_MAX 4
#define SNS_TH 10

// カラーセンサーのアドレス定義
const UB sensor_addr[SENSOR_MAX] = {0x38, 0x39, 0x3c, 0x3D};

// カラーセンサーのデータ構造体
typedef struct {
    UH clear;
    UH red;
    UH green;
    UH blue;
} T_COLOR_DATA;

T_COLOR_DATA cdata[SENSOR_MAX];
INT diff, steer;

ER set_steer(ID dd, INT steer) {
    UW data;
    SZ asz;
    ER err;

    if (steer < -500 || steer > 500) return E_PAR;
    data = steer + 1500; // -500〜+500を 1,000〜2,000 の範囲にする
    err = tk_swri_dev(dd, PMD_DATA_PULSE1, &data, 1, &asz);

    return err;
}

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

// センサー読み取り関数の定義
ER read_sensor(ID sns_dd, T_COLOR_DATA *cdata) {
    T_I2C_EXEC exec;
    UB snd_data;
    SZ asz;
    ER err;

    for (INT i = 0; i < SENSOR_MAX; i++) {
        // センサーのデータ読み取りコマンドを送信
        exec.sadr = sensor_addr[i]; // センサーアドレスを設定
        snd_data = 0x50; // データ読み取りコマンド
        exec.snd_size = 1;
        exec.snd_data = &snd_data;
        exec.rcv_size = sizeof(T_COLOR_DATA);
        exec.rcv_data = (UB*)&cdata[i];

        err = tk_swri_dev(sns_dd, TDN_I2C_EXEC, &exec, sizeof(T_I2C_EXEC), &asz);
        if (err != E_OK) {
            tm_printf("Sensor %d read failed: %d\n", i, err);
            return err;
        }
    }
    return E_OK;
}

LOCAL void drv_task(void *exinf) {
    /* 変数の宣言 */
    ID sns_dd, pmd_dd;
    ER err;
    INT speed;

    sns_dd = tk_opn_dev("iicb", TD_UPDATE);   // I2Cドライバのオープン
    /* 路面センサの初期設定 */
    pmd_dd = tk_opn_dev("pmda", TD_UPDATE);   // PMDドライバのオープン
    err = init_pmd(pmd_dd);                   // PMDデバイスの初期設定
    if (err != E_OK) {
        tm_printf("PMD initialization failed: %d\n", err);
        tk_cls_dev(sns_dd, 0);
        tk_cls_dev(pmd_dd, 0);
        return;
    }

    while (1) {
        err = read_sensor(sns_dd, cdata); // カラーセンサの読込み
        if (err != E_OK) {
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
        set_steer(pmd_dd, steer);         // ステアリング制御

        for (speed = 40; speed < 70; speed++) {
            set_speed(pmd_dd, speed);     // 速度制御
            tm_printf("SPEED = %d\n", speed);
            tk_dly_tsk(1000);             // 1秒待機
        }

        tk_dly_tsk(100);                  // 100msの待機
    }
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
