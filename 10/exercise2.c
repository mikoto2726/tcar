#include <tk/tkernel.h>
#include <tk/device.h> // デバイスの各種宣言、定義
#include <tm/tmonitor.h> // デバッグターミナル出力用

#define SENSOR_MAX 4

// BH1745NUCのアドレス定義
const UB sensor_addr[SENSOR_MAX] = {0x38, 0x39, 0x3c, 0x3D};

// BH1745NUCの初期設定プログラム
void init_sensors(ID dd) {
    UB data[2];
    SZ asz;
    ER err;

    for (INT i = 0; i < SENSOR_MAX; i++) {
        data[0] = 0x40;
        data[1] = 0x80;
        err = tk_swri_dev(dd, sensor_addr[i], data, 2, &asz);

        data[0] = 0x40;
        data[1] = 0x00;
        err = tk_swri_dev(dd, sensor_addr[i], data, 2, &asz);

        data[0] = 0x42;
        data[1] = 0x10;
        err = tk_swri_dev(dd, sensor_addr[i], data, 2, &asz);
    }
}

EXPORT INT usermain(void) {
    ID dd; // デバイスディスクリプタ
    dd = tk_opn_dev("iicb", TD_UPDATE); // デバイスのオープン

    init_sensors(dd); // センサーの初期設定

    for (INT i = 0; i < SENSOR_MAX; i++) {
        while (1) {
            // 測定値更新の確認
            // BH1745NUCのMODE_CONTROL2レジスタを読む
            T_I2C_EXEC exec;
            UB snd_data;
            UB rcv_data;
            SZ asz;
            ER err;

            exec.sadr = sensor_addr[i];
            snd_data = 0x42;
            exec.snd_size = 1;
            exec.snd_data = &snd_data;
            exec.rcv_size = 1;
            exec.rcv_data = &rcv_data;

            err = tk_swri_dev(dd, TDN_I2C_EXEC, &exec, sizeof(T_I2C_EXEC), &asz);
            
            if (rcv_data & (1 << 7)) {
                // BH1745NUCの計測値のレジスタを読む
                UB snd_data;
                UH sens_data[4];

                exec.sadr = sensor_addr[i];
                snd_data = 0x50;
                exec.snd_size = 1;
                exec.snd_data = &snd_data;
                exec.rcv_size = sizeof(sens_data);
                exec.rcv_data = (UB*)(sens_data);

                err = tk_swri_dev(dd, TDN_I2C_EXEC, &exec, sizeof(T_I2C_EXEC), &asz);

                // 測定値をデバッグターミナルに出力
                tm_printf("Sensor %d - Red: %u, Green: %u, Blue: %u, Clear: %u\n", 
                          i, sens_data[0], sens_data[1], sens_data[2], sens_data[3]);
                break;
            }
        }
    }

    tk_cls_dev(dd, 0); // デバイスのクローズ
    return 0;
}
