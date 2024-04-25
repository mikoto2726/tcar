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

/*
 *	cpu_task.h (Simulator)
 *	CPU-Dependent Task Start Processing
 */

#ifndef _SYSDEPEND_TARGET_CPUTASK_
#define _SYSDEPEND_TARGET_CPUTASK_

/*
 * Create stack frame for task startup
 *	Call from 'make_dormant()'
 */
IMPORT void knl_setup_context( TCB *tcb );

/*
 * Set task startup code
 *	Called by 'tk_sta_tsk()' processing.
 */
Inline void knl_setup_stacd( TCB *tcb, INT stacd )
{
	tcb->tskctxb.stacd = stacd;
}

/*
 * Delete task contexts
 */
Inline void knl_cleanup_context( TCB *tcb )
{
}

#endif /* _SYSDEPEND_TARGET_CPUTASK_ */