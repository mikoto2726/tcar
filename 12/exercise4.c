#include <tk/tkernel.h>
#include <tm/tmonitor.h>
#include <tk/device.h> // デバイスの各種宣言、定義

#define SENSOR_MAX 4
#define SNS_TH 10
#define STEER_K 1.0
#define STEER_MAX 500
#define PID_KP 1
#define PID_KI 0
#define PID_KD 0
#define OBSTACLE_THRESHOLD 1000  // 障害物のしきい値

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
INT diff, steer, dir;
INT sns_max, sns_min, sns_now;
UW d; // 障害物検知センサのデータ

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

LOCAL INT pid_control(INT e) {
    static INT last = 0, last_diff = 0, steer = 0;
    INT diff;

    diff = e - last;  // 今回の偏差 - 前回の偏差

    steer += ((PID_KP * diff + PID_KI * e + PID_KD * (diff - last_diff)) / 100);

    last = e;
    last_diff = diff;

    return steer;
}

LOCAL void obstacle_detection_task(void *exinf) {
    while (1) {
        // 障害物検知センサの読み取り
        *(UW*)ADAMOD0 = 0x01;    // 変換開始
        while ((*(UW*)ADAMOD5 & 0x02) == 0);  // 変換完了待ち
        d = *(UW*)ADAREG01 & 0x0fff;  // AINA1の変換結果を読み出す

        /* 変換結果を表示する */
        tm_printf((UB*)"AINA1: %d\n", d);

        // 障害物検知に応じた処理
        if (d > OBSTACLE_THRESHOLD) {
            // 障害物を検知した場合、走行速度を0にする
            set_speed(pmd_dd, 0);
        } else {
            // 障害物が取り除かれた場合、速度を元に戻す
            set_speed(pmd_dd, 50);  // 適当な走行速度に戻す
        }

        tk_dly_tsk(100);  // 100ミリ秒待つ
    }
}

LOCAL void drv_task(void *exinf) {
    /* 変数の宣言 */
    ID sns_dd, pmd_dd;
    ER err;
    INT speed, i, e;

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

    // sns_maxの計算（コース上のセンサー合計値）
    sns_max = 0;
    for (i = 0; i < 10; i++) {
        err = read_sensor(sns_dd, cdata);
        sns_max += cdata[0].clear + cdata[1].clear + cdata[2].clear + cdata[3].clear;
        tk_dly_tsk(100);
    }
    sns_max /= 10;
    sns_min = 100;  // コース外のセンサー合計値（仮の値）

    while (1) {
        err = read_sensor(sns_dd, cdata); // センサーの読み込み
        diff = (cdata[2].clear + cdata[3].clear) - (cdata[0].clear + cdata[1].clear);
        sns_now = cdata[0].clear + cdata[1].clear + cdata[2].clear + cdata[3].clear;

        /* 方向を計算 */
        if (diff > SNS_TH) {
            dir = 1;
        } else if (diff < -SNS_TH) {
            dir = -1;
        } else {
            dir = 0;
        }

        /* 偏差を計算 */
        e = ((sns_max - sns_min) - (sns_now - sns_min)) * 100 / (sns_max - sns_min);
        steer = pid_control(e);
        if (steer > 500) steer = 500;
        steer *= dir;
        set_steer(pmd_dd, steer);  // ステアリングを設定

       

