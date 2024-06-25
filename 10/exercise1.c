#include <tk/tkernel.h>
#include <tk/tmonitor.h>
#include <tk/device.h> // デバイスの各種宣言、定義


//PCA9634のレジスタを書き換えるプログラム
#define HEADLIGHT_ADR 0x18 //ターゲットアドレス 0b0011000
UB data[2]; //送信するデータ
SZ asz; //送信したデータサイズ
ER err; // エラーコード
data[0] = 0x00; //レジスタアドレス
data[1] = 0x00; //レジスタへ設定する値
err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);

//PCA9634のレジスタを初期設定するプログラム
UB data[2];
SZ asz;
ER err;
data[0] = 0; //モードレジスタ1
data[1] = 0x00;
err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
data[0] = 1; //モードレジスタ2
data[1] = 0x05;
err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
data[0] = 0x0c; //LED出力状態(右ヘッドライト)
data[1] = 0x2a;
err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
data[0] = 0x0d; //LED出力状態(左ヘッドライト)
data[1] = 0x2a;
err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);

//ヘッドライトを制御するプログラム
INT unit; //0で左ヘッドライト、1で右ヘッドライト
UB red, green, blue; //RGB各色の値(0~255)
red = 255; green = 0; blue = 0; //全体を赤く光らせる場合の指定
for(unit = 0; unit < 2; unit++){
    data[0] = (unit ? 0x02 : 0x06); // (unit != 0 ? 0x02 :0x06)と同じ
    data[1] = red;
    err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
    
    data[0]++;
    data[1] = green;
    err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);

    data[0]++;
    data[1] = blue;
    err = tk_swri_dev(dd, HEADLIGHT_ADR, &data, 2, &asz);
}

EXPORT INT usermain(void){
    ID dd; //デバイスディスクリプタ
    dd = tk_opn_dev("iicb", TD_UPDATE); //デバイスのオープン

}

