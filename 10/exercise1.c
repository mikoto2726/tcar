#include <tk/tkernel.h>
#include <tk/device.h> // デバイスの各種宣言、定義

// PCA9634のレジスタを書き換えるプログラム
#define HEADLIGHT_ADR 0x18 // ターゲットアドレス 0b0011000

void set_headlight(ID dd) {
    UB data[2];
    SZ asz;
    ER err;
    
    data[0] = 0; // モードレジスタ1
    data[1] = 0x00;
    err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
    
    data[0] = 1; // モードレジスタ2
    data[1] = 0x05;
    err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
    
    data[0] = 0x0c; // LED出力状態(右ヘッドライト)
    data[1] = 0x2a;
    err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
    
    data[0] = 0x0d; // LED出力状態(左ヘッドライト)
    data[1] = 0x2a;
    err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
}

void set_headlight_color(ID dd, UB red, UB green, UB blue) {
    UB data[2];
    SZ asz;
    ER err;
    INT unit;
    for (unit = 0; unit < 2; unit++){
         data[0] = (unit ? 0x02 : 0x06);
         data[1] = red;
         err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
         data[0]++;
         data[1] = green;
         err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
         data[0]++;
         data[1] = blue;
         err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
     }
}

EXPORT INT usermain(void) {
    ID dd; // デバイスディスクリプタ
    dd = tk_opn_dev("iicb", TD_UPDATE); // デバイスのオープン
    UB red, green, blue;

    set_headlight(dd); // PCA9634のレジスタを初期設定するプログラム
    
    // 赤に点灯
    set_headlight_color(dd, 255, 0, 0);
    tk_slp_tsk(1000);

    // 緑に点灯
    set_headlight_color(dd, 0, 255, 0);
    tk_slp_tsk(1000);

    // 青に点灯
    set_headlight_color(dd, 0, 0, 255);
    tk_slp_tsk(1000);

    // 白に点灯
    set_headlight_color(dd, 255, 255, 255);
    tk_slp_tsk(1000);

    // 消灯
    set_headlight_color(dd, 0, 0, 0);
    tk_slp_tsk(1000);

    tk_cls_dev(dd, 0); // デバイスのクローズ
    return 0;
}
