#include <tk/tkernel.h>
#include <tm/tmonitor.h>

#define PEDATA 0x400C0400 //PEデータレジスタのアドレス
#define PE_CR   0x400C0404 //PE出力コントロールレジスタ

ID mtxid;

void task_a(INT stacd, void *exinf){
    *(_UW*)(PE_CR) |= (1 << 3);
    while(1){
        tk_loc_mtx(mtxid, 1, TMO_FEVR);
        for(int i=0; i<10; i++){
            *(_UB*)(PEDATA) |= (1<<3); 
            tk_dly_tsk(500);
            *(_UB*)(PEDATA) &= ~(1<<3);
            tk_dly_tsk(500);
        }
        tk_unl_mtx(mtxid);
    }    
}

void task_b(INT stacd, void *exinf){
    *(_UW*)(PE_CR) |= (1 << 3);
    while(1){
        tk_loc_mtx(mtxid, 1, TMO_FEVR);
        for(int i=0; i<10; i++){
            *(_UB*)(PEDATA) |= (1<<3); 
            tk_dly_tsk(100);
            *(_UB*)(PEDATA) &= ~(1<<3);
            tk_dly_tsk(100);
        }
        tk_unl_mtx(mtxid);
    }    
}

void task_c(INT stacd, void *exinf){
    *(_UW*)(PE_CR) |= (1 << 3);
    while(1){
        tk_loc_mtx(mtxid, 1, TMO_FEVR);
        for(int i=0; i<10; i++){
            *(_UB*)(PEDATA) |= (1<<3); 
            tk_dly_tsk(1000);
            *(_UB*)(PEDATA) &= ~(1<<3);
            tk_dly_tsk(1000);
        }
        tk_unl_mtx(mtxid);
    }    
}

EXPORT int usermain(void){
    T_CTSK ctsk;
    T_CSEM cmtx;
    ID tskid_a, tskid_b, tskid_c;

    cmtx.sematr  = TA_TFIFO;
    cmtx.isemcnt = 1;
    cmtx.maxsem   = 1;
    mtxid = tk_cre_mtx(&cmtx);
 
    ctsk.tskatr  = TA_HLNG | TA_RNG3;
    ctsk.task    = (FP)task_a;
    ctsk.itskpri = 10;
    ctsk.stksz   = 1024;
    tskid_a      = tk_cre_tsk(&ctsk);

    ctsk.task    = (FP)task_b;
    tskid_b      = tk_cre_tsk(&ctsk);

    ctsk.task    = (FP)task_c;
    tskid_c      = tk_cre_tsk(&ctsk);

    tk_sta_tsk(tskid_a, 0);
    tk_sta_tsk(tskid_b, 0);
    tk_sta_tsk(tskid_c, 0);
    tk_sta_tsk(semid, 0);
    
    tk_slp_tsk(TMO_FEVR);
    
    return 0;
}