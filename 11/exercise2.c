#include <tk/tkernel.h>
#include <tm/tmonitor.h>
#include <tk/device.h> // デバイスの各種宣言、定義



//センサーに連携したステアリング制御
#define      SNS_TH      10
T_COLOR_DATA cdata[SENSOR_MAX];
INT           diff, steer;

err = read_sensor(sns_dd, &cdata);
diff = (cdata[2].clear + cdata[3].clear) - (cdata[0].clear + cdata[1].clear);
if(diff > SNS_TH) {              // 左が大きい
    steer = 500;                 // 左折する
} else if(diff < -SNS_TH) {      // 右が大きい
    steer = -500;                // 右折する
} else {                         // 左右同じ
    steer = 0;                   // 直進する
}
set_steer(pmd_dd, steer);


EXPORT INT usermain( void )
{
    T_CTSK   drv_ctsk;          /* 走行タスク生成情報 */
    ER       err;

    /* 走行制御制御タスク生成・実行 */
    drv_ctsk.tskatr = TA_HLNG | TA_RNG3;  /* タスク属性 */
    drv_ctsk.task = (FP)drv_task;         /* 走行タスクの処理関数 */
    drv_ctsk.itskpri = 10;                /* タスク起動時優先度 */
    drv_ctsk.stksz = 1024;                /* スタックサイズ */

    drv_tskid = tk_cre_tsk( &drv_ctsk );  /* 走行タスクの生成 */
    tk_sta_tsk(drv_tskid, 0);             /* 走行タスクの起動 */

    tk_slp_tsk(TMO_FEVR);                 /* 初期タスクを休止状態へ */
                                          /* TMO_FEVRは無限待ち */
    return 0;
}

LOCAL void drv_task(void *exinf)
{
    /* 変数の宣言 */

    sns_dd = tk_opn_dev("iicb", TD_UPDATE);   // I2Cドライバのオープン
    /* 路面センサの初期設定 */
    pmd_dd = tk_opn_dev("pmda", TD_UPDATE);   // PMDドライバのオープン
    /* PMDデバイスの初期設定 */

    while(1) {
        /* センサに連携したステアリング制御 */
        tk_dly_tsk(100);
    }
}
