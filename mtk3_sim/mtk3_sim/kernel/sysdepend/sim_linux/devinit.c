/*
 *----------------------------------------------------------------------
 *    micro T-Kernel 3.0 Simulator
 *
 *    Copyright (C) 2021 by INIAD.
 *    This software is distributed under the T-License 2.2.
 *----------------------------------------------------------------------
 *
 *    Released by TRON Forum(http://www.tron.org) at 2021/02.
 *
 *----------------------------------------------------------------------
 */
#include <sys/machine.h>
#ifdef SIM_LINUX

/*
 *	devinit.c (Simulator)
 *	Device-Dependent Initialization
 */
#include <simconf.h>

#include <stddef.h>
#include <setjmp.h>
#include <signal.h>

#define PROHIBIT_DEF_SIZE_T
#include <sys/sysdef.h>
#include <tm/tmonitor.h>
#include <tk/device.h>

#include "kernel.h"
#include "sysdepend.h"

#include "dispatch.h"

/* ------------------------------------------------------------------------ */
/*
 * System Memory
 */
LOCAL	UB	system_memory[SYSMEM_SIZE];

/* Low level memory manager information */
EXPORT	void	*knl_lowmem_top		= system_memory;		// Head of area (Low address)
EXPORT	void	*knl_lowmem_limit	= &system_memory[SYSMEM_SIZE];	// End of area (High address)

/* ------------------------------------------------------------------------ */
/*
 * Initialization before micro T-Kernel starts
 */

LOCAL T_SIM_TSKENV tskenv_tbl[NUM_TSKID];	// Task execution environment table
LOCAL jmp_buf		sys_env;		// System execution environment

IMPORT TCB		knl_tcb_table[];

LOCAL void create_tskenv_step1(ID tskid);

LOCAL void create_tskenv_step2( ID tskid, UW *p)
{
	TCB		*p_tcb;
	T_SIM_TSKENV	*p_tskenv;

	if( tskid > NUM_TSKID) {
		longjmp(sys_env, 1);	// Task environment creation completed
		/* No return */
	}

	p_tcb = &knl_tcb_table[INDEX_TSK(tskid)];
	p_tskenv = (T_SIM_TSKENV*)p_tcb->tskctxb.tsk_env;

	if(setjmp(p_tskenv->env) == 0) {
		knl_memcpy(p_tskenv->env0, p_tskenv->env, sizeof(jmp_buf));
		p_tskenv->sp_e = p;
		create_tskenv_step1(++tskid);
		/* No return */
	}

	/* ---------------------------------------------------------------------------
		Subsequent programs will be executed by longjmp() from the dispatcher.
		Execution of the first task after creating a task.
	*/
	EI(0);
	p_tcb->task(p_tcb->tskctxb.stacd, p_tcb->exinf);	// Start task execution

	/* ---------------------------------------------------------------------------
		Normally does not return from the task main function.
		If it returns from the function, 
		it causes the task to transition to the dormant state as an abnormality.
	*/
}

LOCAL void create_tskenv_step1(ID tskid)
{
	UW	task_stack[TASK_STACK_SIZE/sizeof(UW)];
	UW	*p;

	p = &(task_stack[0]);
	*p = STACK_MAGIC;

	create_tskenv_step2(tskid, p);
	/* No return */
}

EXPORT ER knl_init_device( void )
{
	T_SIM_TSKENV	*p_tskenv;
	INT		i;

	p_tskenv = tskenv_tbl;
	for(i=0; i < NUM_TSKID; i++, p_tskenv++) {
		knl_tcb_table[i].tskctxb.tsk_env = (void*)p_tskenv;
	}

	if(setjmp(sys_env) == 0) {
		create_tskenv_step1((ID)1);	// Start creating task environment
	}

	knl_startup_hw();

	return E_OK;
}

/* ------------------------------------------------------------------------ */
/*
 * Start processing after T-Kernel starts
 *	Called from the initial task contexts.
 */
LOCAL void dev_cychdr(void *exinf)
{
	static UW	pre_sw3 = (1<<3);
	static UW	pre_sw4 = (1<<7);
	UW		sw3, sw4;

	/* SW3 Interrupt monitoring (in:PA3 out:SIGUSR1) */
	if((*(_UW*)PORTA_IE&(1<<3)) && !(*(_UW*)PORTA_CR&(1<<3)) && (*(_UW*)PORTA_FR3&(1<<3))) {
		sw3 = *(_UW*)PORTA_DATA & (1<<3);
		if(pre_sw3 != sw3) {
			if(!sw3) raise(SIGUSR1);
			pre_sw3 = sw3;
		}
	}

	/* SW4 Interrupt monitoring (in:PE7 out:SIGUSR2) */
	if((*(_UW*)PORTE_IE&(1<<7)) && !(*(_UW*)PORTE_CR&(1<<7)) && (*(_UW*)PORTE_FR3&(1<<7))) {
		sw4 = *(_UW*)PORTE_DATA & (1<<7);
		if(pre_sw4 != sw4) {
			if(!sw4) raise(SIGUSR2);
			pre_sw4 = sw4;
		}
	}
}

EXPORT ER knl_start_device( void )
{
	T_CCYC	ccyc;
	ID	cycid;
	ER	err;

	/* Virtual device monitoring */
	ccyc.cycatr	= TA_HLNG;
	ccyc.cychdr	= dev_cychdr;
	ccyc.cyctim	= 100;
	
	cycid = tk_cre_cyc(&ccyc);
	tk_sta_cyc(cycid);

#if USE_SDEV_DRV	// Use sample driver

	/* I2C SBI1 "iicb" */
	#if DEVCNF_DEV_IIC
		err = dev_init_i2c(1);
		if(err < E_OK) return err;
	#endif

#endif
	err = dev_init_pmd();
	if(err < E_OK) return err;

	return E_OK;
}

#if USE_SHUTDOWN
/* ------------------------------------------------------------------------ */
/*
 * System finalization
 *	Called just before system shutdown.
 *	Execute finalization that must be done before system shutdown.
 */
EXPORT ER knl_finish_device( void )
{
	return E_OK;
}

#endif /* USE_SHUTDOWN */

#endif /* SIM_LINUX */
