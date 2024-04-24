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
 *	interrupt.c (Simulator)
 *	Interrupt control
 */
#include <simconf.h>

#include <signal.h>

#define PROHIBIT_DEF_SIZE_T
#include "kernel.h"
#include "sysdepend.h"
#include "sys_timer.h"

/* 
	knl_int_level : Interrupt level
		Level 0: Interrupt enable (INTLEVEL_EI)
		Level 1: Interrupt disable (INTLEVEL_DI)
 */
EXPORT UW knl_int_level	= INTLEVEL_EI;
EXPORT UW knl_int_mask	= 0;

/* ------------------------------------------------------------------------ */
/*
 * HLL(High level programming language) Interrupt Handler
 */

LOCAL FP knl_inthdr_tbl[N_INTVEC];	/* HLL Interrupt Handler Table */

EXPORT void knl_hll_inthdr(int signo)
{
	FP	inthdr;

	BEGIN_CRITICAL_SECTION;
	ENTER_TASK_INDEPENDENT;

	inthdr	= knl_inthdr_tbl[signo];
	(*inthdr)(signo);

	LEAVE_TASK_INDEPENDENT;
	END_CRITICAL_SECTION;
}

/* ----------------------------------------------------------------------- */
/*
 * Set interrupt handler (Used in tk_def_int())
 */
EXPORT ER knl_define_inthdr( INT intno, ATR intatr, FP inthdr )
{
	if(intno == 0)	return E_PAR;

	if(inthdr != NULL) {
		if ( (intatr & TA_HLNG) != 0 ) {
			knl_inthdr_tbl[intno] = inthdr;
			inthdr = knl_hll_inthdr;
		}
		signal(intno, inthdr);
	} else 	{	/* Clear interrupt handler */
		signal(intno, SIG_DFL);
	}

	return E_OK;
}

/* ----------------------------------------------------------------------- */
/*
 * Return interrupt handler (Used in tk_ret_int())
 */
EXPORT void knl_return_inthdr(void)
{
	/* No processing in ARM. */
	return;
}

/* ------------------------------------------------------------------------ */
/*
 * Interrupt initialize
 */
EXPORT ER knl_init_interrupt( void )
{
	/* Mask signal (SIGUSR1 & SIGUSR1) */
	DisableInt(SIGUSR1);
	DisableInt(SIGUSR2);

	ENABLE_INTERRUPT;
	return E_OK;
}


#endif	/* CPU_CORE_ARMV7M */