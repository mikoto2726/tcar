#include <tk/tkernel.h>  /* T-Kernel ヘッダ */
#include <tm/tmonitor.h> /* tm_printf() など */

// GPIOポートA
#define GPIOA_BASE (0x400C0000)
#define PADATA  (GPIOA_BASE + 0x00)
#define PACR    (GPIOA_BASE + 0x04)
#define PAIE    (GPIOA_BASE + 0x38)

// GPIOポートH
#define GPIOH_BASE (0x400C0700)
#define PHCR    (GPIOH_BASE + 0x04)
#define PHFR3   (GPIOH_BASE + 0x10)
#define PHIE    (GPIOH_BASE + 0x38)

// MPT2タイマ
#define	MT2_BASE	(0x400C7200UL)
#define	MT2EN   (MT2_BASE + 0x00)
#define	MT2RUN	(MT2_BASE + 0x04)
#define	MT2IGCR	(MT2_BASE + 0x30)
#define	MT2IGOCR	(MT2_BASE + 0x40)
#define	MT2IGRG2	(MT2_BASE + 0x44)
#define	MT2IGRG3	(MT2_BASE + 0x48)
#define	MT2IGRG4	(MT2_BASE + 0x4C)

/* ブザー制御タスクの処理関数 */
void bz_task(INT stacd, void *exinf)
{
    UW sw3_prev = 0;  // 最初のSW3の状態を保持する変数

      // MTP2初期設定
    *(_UW*)MT2EN     |=  (1 << 7) | (1 << 0);
    *(_UW*)MT2IGOCR  |= (1 << 1);
    *(_UW*)MT2IGOCR  &= ~(1 << 5);
    *(_UW*)MT2IGCR   &= ~(1 << 6);
    *(_UW*)MT2IGCR   &= 0xF0;

    // GPIOを入力ポートに設定 初期化 端子
    *(_UW*)PACR &= ~(1 << 3);
    *(_UW*)PAIE |= (1 << 3);
      // 端子をMTP2に設定
    *(_UW*)PHFR3   |=  (1 << 2);
    *(_UW*)PHIE    &= ~(1 << 2);
    *(_UW*)PHCR   |=  (1 << 2);
  

    

      // MTP2 出力波形の設定 
    *(_UW*)MT2IGRG2  = 1;
    *(_UW*)MT2IGRG4  = 18000;
    *(_UW*)MT2IGRG3  = 9000;

    while(1) {
        UW sw3 = *(_UW*)PADATA & (1 << 3);  // SW3の状態を読み取る
        if (sw3 != sw3_prev) {  // SW3の状態が変化したとき
            sw3_prev = sw3;  // 前回の状態を更新
            if (sw3 == 0) {  // SW3が押されたとき
                // MPT2からブザーに波形を出力するコード
                *(_UW*)MT2RUN |= (1 << 2);  // MPT2の出力2番をONにする
                *(_UW*)MT2RUN |= (1 << 0);  // MPT2の出力0番をONにする
                tk_dly_tsk(2000);  // 2秒間待機
                *(_UW*)MT2RUN &= ~(1 << 2);  // MPT2の出力2番をOFFにする
                *(_UW*)MT2RUN &= ~(1 << 0);  // MPT2の出力0番をOFFにする
            }
        }
        tk_dly_tsk(100);  // 一定時間待機
    }
}

EXPORT INT usermain(void)
{
    T_CTSK bz_ctsk;
    ID bz_tskid;

    /* タスク生成情報 */
    /* タスクID */

    bz_ctsk.tskatr = TA_HLNG | TA_RNG3; /* タスク属性 */
    bz_ctsk.task = bz_task;
    bz_ctsk.itskpri = 10; /* タスク起動時優先度 */
    bz_ctsk.stksz = 1024; /* スタックサイズ */

    bz_tskid = tk_cre_tsk(&bz_ctsk);
    tk_sta_tsk(bz_tskid, 0); /* ブザー制御タスクの生成と起動 */

    tk_slp_tsk(TMO_FEVR); /* 初期タスクを休止状態へ（無限待ち） */
    return 0;
}
