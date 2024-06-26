#include <tk/tkernel.h>
#include <tk/tmonitor.h>
#include <tk/device.h> // デバイスの各種宣言、定義

#define SENSOR_MAX 4

//BH1745NUCの定義の仕方
const UB sensor_addr[SENSOR_MAX] = {0x38, 0x39, 0x3c, 0x3D}

//BH1745NUCの初期設定プログラム
UB data[2];
SZ asz;
INT i;
ER err;

for(i=0; i<SENSOR_MAX; i++){
    data[0] = 0x40;
    data[1] = 0x80;

    err = tk_swri_dev(dd, sensor_addr[i], &data, 2, &asz);

    data[0] = 0x40;
    data[1] = 0x00;
    err = tk_swri_dev(dd, sensor_addr[i] , &data, 2, &asz);

    data[0] = 0x42;
    data[1] = 0x10;
    err = tk_swri_dev(dd, sensor_addr[i] , &data, 2, &asz);
}

typedef struct{
    UW sadr;
    W  snd_size;
    UB *snd_data;
    W rcv_size;
    UB *rcv_data;
}T_I2C_EXEC;


//BH1745NUCのMODE_CONTROL2レジスタを読む
T_I2C_EXEC exec;
UB         snd_data;
UB         rcv_data;
SZ         asz;
ER         err;

exec.sadr = sensor_addr[i];
snd_data  = 0x42;
exec.snd_size = 1;
exec.snd_data = &snd_data;
exec.rcv_size = 1;
exec.rcv_data = &rcv_data;

err = tk_swri_dev(dd, TDN_I2C_EXEC, &exec, sizeof(T_I2C_EXEC), &asz);