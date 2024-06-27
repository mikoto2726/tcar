#include <tk/tkernel.h>
#include <tm/tmonitor.h>
#include <tk/device.h> // デバイスの各種宣言、定義


//PMDデバイスpmdaを使用開始するプログラム
EXPORT INT usermain( void ){
    ID dd; // デバイスディスクリプタ

    dd = tk_opn_dev("pmda", TD_UPDATE); // デバイスのオープン

    /* 他のコード */
}


//PMD初期設定
UW  data;
SZ  asz;
ER  err;

data = 14000;        // 矩形波の周期 14,000μ秒
err = tk_swri_dev( dd, PMD_DATA_PERIOD, &data, 1, &asz);

data = 14000-1500;   // 走行モータの初期値(停止)
err = tk_swri_dev( dd, PMD_DATA_PULSE0, &data, 1, &asz);

data = 1500;         // ステアリングの初期値(直進)
err = tk_swri_dev( dd, PMD_DATA_PULSE1, &data, 1, &asz);

data = 1;            // 出力開始
err = tk_swri_dev( dd, PMD_DATA_EN, &data, 1, &asz);

//ステアリング操作関数
ER set_steer(ID dd, INT steer)
{
    UW data;
    SZ asz;
    ER err;

    if(steer < -500 || steer > 500) return E_PAR;
    data = steer + 1500; // -500〜+500を 1,000〜2,000 の範囲にする
    err = tk_swri_dev( dd, PMD_DATA_PULSE1, &data, 1, &asz);

    return err;
}

//ステアリングの操作実験
while(1) {
    while(steer < 500) {
        steer += 100;
        set_steer(dd, steer);
        tk_dly_tsk(500);
    }
    while(steer > -500) {
        steer -= 100;
        set_steer(dd, steer);
        tk_dly_tsk(500);
    }
}
