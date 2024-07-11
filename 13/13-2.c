#include <tk/tkernel.h>
#include <tm/tmonitor.h>
#include <tk/device.h>
#include <utkn/utkn.h>
#include <utkn/coap/coap.h>

#define SENSOR_MAX 4
#define SNS_TH 10
#define STEER_K_NUM 1000  // スケーリングファクターを使用
#define STEER_MAX 500

// PID制御の定数
#define PID_KP 1000
#define PID_KI 100
#define PID_KD 10

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

// PID制御の関数宣言
LOCAL INT pid_control(INT e);

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
    INT speed, i;

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

    // CoAPサーバの起動
    coap_server_start();

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

        /* PID制御によるステアリング制御 */
        steer = pid_control(diff);  // PID制御関数を呼び出し

        if (steer > STEER_MAX) steer = STEER_MAX;
        if (steer < -STEER_MAX) steer = -STEER_MAX;

        set_steer(pmd_dd, steer);  // ステアリングを設定

        for (speed = 50; speed < 51; speed++) {
            set_speed(pmd_dd, speed);     // 速度制御
            tm_printf("SPEED = %d\n", speed);
            tk_dly_tsk(1000);             // 1秒待機
        }

        tk_dly_tsk(100);  // 0.1秒周期で繰り返す
    }
}

LOCAL INT pid_control(INT e) {
    static INT last = 0, last_diff = 0, steer = 0;
    INT diff;

    diff = e - last;  // 今回の偏差 - 前回の偏差

    steer += (PID_KP * diff + PID_KI * e + PID_KD * (diff - last_diff)) / 100;

    last = e;
    last_diff = diff;

    return steer;
}

static void coap_server_task(void *exinf) {
    static coap_msg_t req, rpl;
    static UB buf[128];
    ER err;
    coap_addr_t ip6_addr;

    // CoAPサービスの初期化
    static coap_init_t coap;
    coap.maxreqq = 16;
    coap.maxlogq = 128;
    coap.maxfsz = 256;

    // IPアドレスを設定
    static const UB addr[] = {
        0x20, 0x01, 0x0d, 0xf7, 0x56, 0x00, 0x80, 0x02,
        0x21, 0x0d, 0x12, 0x90, 0x00, 0x04, 0x37, 0xcb
    };
    memcpy(ip6_addr.in6_addr.s6_addr, addr, sizeof(addr));
    ip6_addr.port = htons(COAP_PORT_DEFAULT);

    coap.cep.addr = ip6_addr;

    err = coap_init(&coap);
    if (err != E_OK) {
        tm_printf("CoAP initialization failed: %d\n", err);
        return;
    }

    while (1) {
        // CoAP要求の受信
        err = coap_accept(&req, TMO_FEVR);
        if (err != E_OK) {
            tm_printf("CoAP accept failed: %d\n", err);
            continue;
        }

        if (req.code == COAP_METHOD_GET) {
            if (coap_option_isequal(req.uri_path, "name")) {
                // 応答メッセージの作成
                coap_msg_init(&rpl, 0);
                rpl.t = req.t;
                rpl.token = req.token;
                rpl.dst = req.dst;
                rpl.body = buf;
                rpl.bodymax = sizeof(buf);
                rpl.bodysz = tm_sprintf(rpl.body, (UB*)"MY T-Car");
                rpl.code = COAP_CODE_CONTENT;
            } else {
                rpl.code = COAP_CODE_NOT_FOUND;
                rpl.bodysz = -1;
            }
        } else {
            rpl.code = COAP_CODE_METHOD_NOT_ALLOWED;
            rpl.bodysz = -1;
        }

        // CoAP応答の送信
        err = coap_reply(&rpl, TMO_FEVR);
        if (err != E_OK) {
            tm_printf("CoAP reply failed: %d\n", err);
        }
    }
}

void coap_server_start() {
    T_CTSK coap_ctsk;
    ID coap_tskid;

    coap_ctsk.tskatr = TA_HLNG | TA_RNG3;
    coap_ctsk.task = (FP)coap_server_task;
    coap_ctsk.itskpri = 10;
    coap_ctsk.stksz = 1024;

    coap_tskid = tk_cre_tsk(&coap_ctsk);
    tk_sta_tsk(coap_tskid, 0);
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
