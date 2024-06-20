#include <tk/tkernel.h>
#include <tm/tmonitor.h>

void almhdr(void *exinf){
    tm_printf("almhdr"); 
    return; //アラームハンドラの終了
}

EXPORT int usermain(void) {
T_CALM  calm; //アラームハンドラ生成情報
ID      almid; //アラームハンドラID番号

calm.almatr = TA_HLNG;
calm.almhdr = almhdr;
almid = tk_cre_alm(&calm);
tk_sta_alm(almid, 3000);
tk_slp_tsk(TMO_FEVR);// アラームハンドラが実行されるのを待つ

    return 0;
}