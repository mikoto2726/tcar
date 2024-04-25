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
 *	cpu_cntl.c (Simulator)
 *	CPU-Dependent Control
 */
#include <simconf.h>

#include <stddef.h>
#include <setjmp.h>

#define PROHIBIT_DEF_SIZE_T
#include "kernel.h"
#include "sysdepend.h"
#include "cpu_task.h"
#include "dispatch.h"

/* Task independent status */
EXPORT	W	knl_taskindp = 0;

/* ----------------------------------------------------------------------- */
/*
 *	Task dispatcher
 */
EXPORT void knl_force_dispatch( void )
{
	knl_dispatch_disabled = DDS_DISABLE_IMPLICIT;
	knl_ctxtsk = NULL;

	knl_dispatch();		/* Execute dispatch */
}

EXPORT void knl_dispatch( void )
{
	T_SIM_TSKENV	*p_ctskenv, *p_stskenv;
	UW		*sp;

	knl_dispatch_disabled = DDS_DISABLE_IMPLICIT;

	if(knl_ctxtsk != NULL) {
		p_ctskenv = (T_SIM_TSKENV*)knl_ctxtsk->tskctxb.tsk_env;

		/* Check for task overflow */
		sp = p_ctskenv->sp_e;
		if(*sp != STACK_MAGIC) {
			tm_printf((UB*)"STACK OVERFLOW! %d\n", (int)*sp);
		}
	} else {
		p_ctskenv = NULL;
	}

	while (knl_schedtsk == NULL) {
		low_pow();
	}

	p_stskenv = (T_SIM_TSKENV*)knl_schedtsk->tskctxb.tsk_env;
	knl_ctxtsk = knl_schedtsk;
	knl_dispatch_disabled = DDS_ENABLE;
	if(p_ctskenv != NULL) {
		if(setjmp(p_ctskenv->env) == 0) {
			longjmp(p_stskenv->env, 1);
			/* No return */
		}
	} else {
		longjmp(p_stskenv->env, 1);
		/* No return */
	}
}

/*
 * Create stack frame for task startup
 *	Call from 'make_dormant()'
 */
EXPORT void knl_setup_context( TCB *tcb )
{
	T_SIM_TSKENV	*p_tskenv;

	p_tskenv = (T_SIM_TSKENV*)tcb->tskctxb.tsk_env;
	knl_memcpy(p_tskenv->env, p_tskenv->env0, sizeof(jmp_buf));
}

#endif /* SIM_LINUX */
