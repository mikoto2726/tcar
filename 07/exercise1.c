#include <tk/tkernel.h>
#include <tm/tmonitor.h>


// GPIOポートA sw3
#define GPIOA_BASE (0x400C0000)
#define PADATA  (GPIOA_BASE + 0x00)
#define PACR    (GPIOA_BASE + 0x04)
#define PAIE    (GPIOA_BASE + 0x38)

// GPIOポートE sw4
#define GPIOE_BASE (0x400C0400)
#define PEDATA  (GPIOE_BASE + 0x00)
#define PECR    (GPIOE_BASE + 0x04)
#define PEIE    (GPIOE_BASE + 0x38)

#define PE_DATA 0x400C0400 //PEデータレジスタのアドレス
#define PE_CR   0x400C0404 //PE出力コントロールレジスタ

typedef struct{
	INT sw;
	SYSTIM time;
}T_MSG_SW;

ID mbfid ,led_tskid;
T_MSG_SW msg;



void sw_task(INT stack, void *exinf){
	UW sw3, sw4;
	*(_UW*)PACR &= ~(1<<3);
	*(_UW*)PAIE |= (1<<3);
	*(_UW*)PECR &= ~(1<<7);
	*(_UW*)PEIE |= (1<<7);

	while(1){
		sw3 = *(_UW*)PADATA &(1<<3);
		if(sw3 == 0){
			msg.sw = 3;
			tk_get_tim(&msg.time);
			tk_snd_mbf(mbfid, &msg, sizeof(T_MSG_SW), TMO_FEVR);
		}
		sw4 = *(_UW*)PEDATA & (1<<7);
		if(sw4 == 0){
			msg.sw = 4;
			tk_get_tim(&msg.time);
			tk_snd_mbf(mbfid, &msg, sizeof(T_MSG_SW), TMO_FEVR);
		}
		tk_dly_tsk(100);
	}
}

void led_task(INT stack, void *exinf){
	*(_UW*)(PE_CR) |=(1<<2);
	*(_UW*)(PE_CR) |=(1<<3);

	while(1){
		tk_rcv_mbf(mbfid, &msg, TMO_FEVR);
		if(msg.sw == 3){
			*(_UB*)(PE_DATA) |= (1<<3);
			tk_dly_tsk(1000);
			tm_printf("sw3 pressed at time: %llu\n", msg.time);
			*(_UB*)(PE_DATA) &= ~(1<<3);
		}
		else if (msg.sw == 4){
			*(_UB*)(PE_DATA) |= (1<<2);
			tk_dly_tsk(1000);
			tm_printf("sw4 pressed at time: %llu\n", msg.time);
			*(_UB*)(PE_DATA) &= ~(1<<2);
		}
	}
}


EXPORT int usermain(void){
	T_CMBF cmbf;
	T_CTSK ctsk;
	ID tskid1;

	cmbf.mbfatr = TA_TFIFO;
	cmbf.bufsz  = sizeof(T_MSG_SW)*10;
    cmbf.maxmsz = sizeof(T_MSG_SW);

	mbfid = tk_cre_mbf(&cmbf);

	ctsk.tskatr  = TA_HLNG | TA_RNG3;
  	ctsk.task    = (FP)sw_task;
  	ctsk.itskpri = 10;
  	ctsk.stksz   = 1024;
  	tskid1       = tk_cre_tsk( &ctsk );  

  	ctsk.task    = (FP)led_task;
  	ctsk.itskpri = 10;
  	led_tskid    = tk_cre_tsk( &ctsk );  

  	tk_sta_tsk(tskid1, 0);
  	tk_sta_tsk(led_tskid, 0);  
  	tk_slp_tsk(TMO_FEVR);
  	return 0;	
}