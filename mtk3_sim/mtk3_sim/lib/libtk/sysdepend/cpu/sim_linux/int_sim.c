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
 *	int_sim.c
 *
 *	Interrupt controller (Simulator )
 */
#include <signal.h>
#define PROHIBIT_DEF_SIZE_T

#include <tk/tkernel.h>
#include <tk/syslib.h>


IMPORT UW knl_int_level;
IMPORT UW knl_int_mask;

/*----------------------------------------------------------------------*/
/*
 * CPU Interrupt Control API
 *
 */

/* 
 * Enable interrupt
 */
EXPORT void enaint(UW intsts)
{
	sigset_t	set;

	sigfillset(&set);
	sigprocmask(SIG_SETMASK, &set, NULL);	/* Begin critical section */

	knl_int_level = intsts;
	if (knl_int_level == INTLEVEL_EI) {
		sigemptyset(&set);
		for(INT i = 1; i < N_INTVEC; i++) {
			if(knl_int_mask & (1<<i)) sigaddset(&set, i);
		}
		sigprocmask(SIG_SETMASK, &set, NULL);
	}
}

/* 
 * Disable interrupt 
 */
EXPORT UW disint(void)
{
	sigset_t	set;
	UW		rtncd;

	sigfillset(&set);
	sigprocmask(SIG_SETMASK, &set, NULL);

	rtncd = knl_int_level;
	knl_int_level = INTLEVEL_DI;

	return rtncd;
}

/*
 * Is interrupt disabled?
 */
EXPORT UW is_disint(void)
{
	return (knl_int_level == INTLEVEL_DI);
}

/*----------------------------------------------------------------------*/
/*
 * Interrupt control API
 * 
 */

/*
 * Enable interrupt 
 */
EXPORT void EnableInt( UINT intno, INT level )
{
	sigset_t	all, set;

	if(intno <= 0 || intno >=  N_INTVEC) return;

	sigfillset(&all);
	sigprocmask(SIG_SETMASK, &all, &set); /* Begin critical section */

	knl_int_mask &= ~(1<< intno);

	if(knl_int_level == INTLEVEL_EI) {
		sigdelset(&set, intno);
	}

	sigprocmask(SIG_SETMASK, &set, NULL); /* End critical section */
}

/*
 * Disable interrupt 
 */
EXPORT void DisableInt( UINT intno )
{
	sigset_t	all, set;

	if(intno <= 0 || intno >=  N_INTVEC) return;

	sigfillset(&all);
	sigprocmask(SIG_SETMASK, &all, &set); /* Begin critical section */

	knl_int_mask |= (1<< intno);

	if(knl_int_level == INTLEVEL_EI) {
		sigaddset(&set, intno);
	}

	sigprocmask(SIG_SETMASK, &set, NULL); /* End critical section */
}

/*
 * Clear interrupt
 */
EXPORT void ClearInt(UINT intno)
{
}

/*
 * Set interrupt mode
 */
EXPORT void SetIntMode(UINT intno, UINT mode)
{
}

#endif /* SIM_LINUX */
