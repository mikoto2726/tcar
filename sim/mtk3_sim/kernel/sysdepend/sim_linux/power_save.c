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
 *	power_save.c (Simulator)
 *	Power-Saving Function
 */
#include <simconf.h>

#include <signal.h>

#define PROHIBIT_DEF_SIZE_T
#include <tk/tkernel.h>
#include <kernel.h>

#include "sysdepend.h"

IMPORT UW knl_int_mask;

/*
 * Switch to power-saving mode
 */
EXPORT void low_pow( void )
{
	sigset_t	set;

	sigemptyset(&set);
	for(INT i = 1; i < N_INTVEC; i++) {
		if(knl_int_mask & (1<<i)) sigaddset(&set, i);
	}
	sigsuspend(&set);
}

/*
 * Move to suspend mode
 */
EXPORT void off_pow( void )
{
	sigset_t	set;

	sigemptyset(&set);
	for(INT i = 1; i < N_INTVEC; i++) {
		if(knl_int_mask & (1<<i)) sigaddset(&set, i);
	}
	sigsuspend(&set);
}


#endif /* SIM_LINUX */
