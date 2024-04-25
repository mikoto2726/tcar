#include <tk/tkernel.h>
#include <tm/tmonitor.h>

EXPORT  usermain(void){
	ID tskid;
	tskid = tk_get_tid();
	tm_printf("Task ID:%d\n", (INT)tskid);
	return 0;
}

 	