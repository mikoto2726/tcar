#include <tk/tkernel.h>
#include <tm/tmonitor.h>

#define PEDATA 0x400C0400 //PEデータレジスタのアドレス
#define PE_CR   0x400C0404 //PE出力コントロールレジスタ

void task_a(INT stacd, void *exinf){
    *(_UW*)(PE_CR) |= (1 << 3);
    while(1){
            *(_UB*)(PEDATA) |= (1<<3); 
            tk_dly_tsk(500);
            *(_UB*)(PEDATA) &= ~(1<<3);
            tk_dly_tsk(500);
    }    
}

void task_b(INT stacd, void *exinf){
    *(_UW*)(PE_CR) |= (1 << 3);
    while(1){
            *(_UB*)(PEDATA) |= (1<<3); 
            tk_dly_tsk(100);
            *(_UB*)(PEDATA) &= ~(1<<3);
            tk_dly_tsk(100);
    }    
}

void task_c(INT stacd, void *exinf){
    *(_UW*)(PE_CR) |= (1 << 3);
    while(1){
            *(_UB*)(PEDATA) |= (1<<3); 
            tk_dly_tsk(1000);
            *(_UB*)(PEDATA) &= ~(1<<3);
            tk_dly_tsk(1000);
    }    
}

EXPORT int usermain(void){
    T_CTSK ctsk;
    ID tskid_a, tskid_b, tskid_c;

    ctsk.tskatr  = TA_HLNG | TA_RNG3;
    ctsk.task    = (FP)task_a;
    ctsk.itskpri = 10;
    ctsk.stksz   = 1024;
    tskid_a      = tk_cre_tsk(&ctsk);

    ctsk.task    = (FP)task_b;
    ctsk.itskpri = 10;
    tskid_b      = tk_cre_tsk(&ctsk);

    ctsk.task    = (FP)task_c;
    ctsk.itskpri = 10;
    tskid_c      = tk_cre_tsk(&ctsk);

    tk_sta_tsk(tskid_a, 0);
    tk_sta_tsk(tskid_b, 0);
    tk_sta_tsk(tskid_c, 0);
    
    tk_slp_tsk(TMO_FEVR);
    return 0;
}