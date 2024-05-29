 #include <tk/tkernel.h>
#include <tk/device.h>

// グローバル変数
ID sw_flgid;

// 初期タスク
EXPORT INT usermain(void) {
    T_CFLG cflg;
    cflg.flgatr = TA_WMUL | TA_TFIFO;
    cflg.iflgptn = 0;
    sw_flgid = tk_cre_flg(&cflg);

    // SW監視タスクの生成・実行
    T_CTSK ctsk;
    ctsk.tskatr = TA_HLNG | TA_RNG3;
    ctsk.task = (FP)sw_monitor_task;
    ctsk.itskpri = 10;
    ctsk.stksz = 1024;
    tk_cre_tsk(&ctsk);
    tk_sta_tsk(&ctsk, 0);

    // LED制御タスクの生成・実行
    ctsk.task = (FP)led_control_task;
    tk_cre_tsk(&ctsk);
    tk_sta_tsk(&ctsk, 0);

    // 無限待ち
    tk_slp_tsk(TMO_FEVR);
    return 0;
}
void sw_monitor_task(void) {
    while (1) {
        _UW sw3 = *(_UW*)PADATA & (1 << 3); // SW3の読み込み
        if (sw3 == 0) { // SW3押下?
            tk_set_flg(sw_flgid, (1 << 0)); // ビット0のフラグをセット
        }

        _UW sw4 = *(_UW*)PEDATA & (1 << 7); // SW4の読み込み
        if (sw4 == 0) { // SW4押下?
            tk_set_flg(sw_flgid, (1 << 1)); // ビット1のフラグをセット
        }

        tk_dly_tsk(100); // 0.1秒待つ
    }
}
void led_control_task(void) {
    UINT flg = 0;
    while (1) {
        tk_wai_flg(sw_flgid, (1 << 0) | (1 << 1), (TWF_ORW | TWF_BITCLR), &flg, TMO_FEVR);

        if (flg & (1 << 0)) { // SW3が押下?
            *(_UB*)(PEDATA) |= (1 << 3); // 左LED (PE3)点灯
            tk_dly_tsk(1000); // 1秒間待つ
            *(_UB*)(PEDATA) &= ~(1 << 3); // 左LED (PE3)消灯
        }

        if (flg & (1 << 1)) { // SW4が押下?
            *(_UB*)(PEDATA) |= (1 << 2); // 右LED (PE2)点灯
            tk_dly_tsk(1000); // 1秒間待つ
            *(_UB*)(PEDATA) &= ~(1 << 2); // 右LED (PE2)消灯
        }
    }
}
