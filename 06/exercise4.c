#include <tk/tkernel.h>
#include <tm/tmonitor.h>

#define PEDATA 0x400C0400 //PEデータレジスタのアドレス
#define PE_CR   0x400C0404 //PE出力コントロールレジスタ

ID mtxid;

void task_a(INT stacd, void *exinf){
    T_RTSK rtsk;
    *(_UW*)(PE_CR) |= (1 << 3);
    while(1){
        tk_ref_tsk(TSK_SELF, &rtsk);
        printf("Task A before lock: Priority = %d\n", rtsk.tskpri);

        tk_loc_mtx(mtxid, 1, TMO_FEVR);

        tk_ref_tsk(TSK_SELF, &rtsk);
        printf("Task A after lock: Priority = %d\n", rtsk.tskpri);

        for(int i=0; i<10; i++){
            *(_UB*)(PEDATA) |= (1<<3); 
            tk_dly_tsk(500);
            *(_UB*)(PEDATA) &= ~(1<<3);
            tk_dly_tsk(500);
        }
        tk_unl_mtx(mtxid);

        tk_ref_tsk(TSK_SELF, &rtsk);
        printf("Task B after unlock: Priority = %d\n", rtsk.tskpri);

    }    
}

void task_b(INT stacd, void *exinf){
    T_RTSK rtsk;
    *(_UW*)(PE_CR) |= (1 << 3);
    while(1){
        tk_ref_tsk(TSK_SELF, &rtsk);
        printf("Task B before lock: Priority = %d\n", rtsk.tskpri);

        tk_loc_mtx(mtxid, 1, TMO_FEVR);

        tk_ref_tsk(TSK_SELF, &rtsk);
        printf("Task B after lock: Priority = %d\n", rtsk.tskpri);

        for(int i=0; i<10; i++){
            *(_UB*)(PEDATA) |= (1<<3); 
            tk_dly_tsk(100);
            *(_UB*)(PEDATA) &= ~(1<<3);
            tk_dly_tsk(100);
        }
        tk_unl_mtx(mtxid);

        tk_ref_tsk(TSK_SELF, &rtsk);
        printf("Task B after unlock: Priority = %d\n", rtsk.tskpri);
    }    
}

void task_c(INT stacd, void *exinf){
    T_RTSK rtsk;
    *(_UW*)(PE_CR) |= (1 << 3);
    while(1){
        tk_ref_tsk(TSK_SELF, &rtsk);
        printf("Task C before lock: Priority = %d\n", rtsk.tskpri);

        tk_loc_mtx(mtxid, 1, TMO_FEVR);

        tk_ref_tsk(TSK_SELF, &rtsk);
        printf("Task C after lock: Priority = %d\n", rtsk.tskpri);

        for(int i=0; i<10; i++){
            *(_UB*)(PEDATA) |= (1<<3); 
            tk_dly_tsk(1000);
            *(_UB*)(PEDATA) &= ~(1<<3);
            tk_dly_tsk(1000);
        }
        tk_unl_mtx(mtxid);
        tk_ref_tsk(TSK_SELF, &rtsk);
        printf("Task C after lock: Priority = %d\n", rtsk.tskpri);
    }    
}

EXPORT int usermain(void){
    T_CTSK ctsk;
    T_CMTX cmtx;
    ID tskid_a, tskid_b, tskid_c;

    cmtx.sematr  = TA_INHERIT;
    cmtx.maxsem   = 8;
    mtxid = tk_cre_mtx(&cmtx);
 
    ctsk.tskatr  = TA_HLNG | TA_RNG3;
    ctsk.task    = (FP)task_a;
    ctsk.itskpri = 10;
    ctsk.stksz   = 1024;
    tskid_a      = tk_cre_tsk(&ctsk);

    ctsk.task    = (FP)task_b;
    ctsk.itskpri = 10;
    tskid_b      = tk_cre_tsk(&ctsk);

    ctsk.task    = (FP)task_c;
    ctsk.itskpri = 8;
    tskid_c      = tk_cre_tsk(&ctsk);

    tk_sta_tsk(tskid_a, 0);
    tk_sta_tsk(tskid_b, 0);
    tk_sta_tsk(tskid_c, 0);
    tk_sta_tsk(mtxid, 0);
    
    tk_slp_tsk(TMO_FEVR);
    
    return 0;
}