
/* ブザー制御タスクの処理関数 */
void bz_task(INT stacd, void *exinf)
{
     // GPIOを入力ポートに設定
    *(_UW*)PACR &= ~(1<<3);
    *(_UW*)PAIE |= (1<<3);
    *(_UW*)PECR &= ~(1<<7);
    *(_UW*)PEIE |= (1<<7);

    while(1) {
        sw3 = *(_UW*)PADATA & (1<<3);
        tm_printf("SW3 = %d  SW4 = %d\n", sw3, sw4);
        tk_dly_tsk(500);
    }
}


EXPORT INT usermain(void) {
}

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

