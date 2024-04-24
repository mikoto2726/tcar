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
 *	sysdepend.h (Simulator)
 *	System-Dependent local defined
 */
#ifndef _SYSDEPEND_TARGET_SYSDEPEND_
#define _SYSDEPEND_TARGET_SYSDEPEND_

/*
 * Task context block
 */
typedef struct {
	INT	stacd;
	void	*tsk_env;	// Task execution environment
} CTXB;

/*
 *    Clock Setting (cpu_clock.c)
 */
IMPORT void startup_clock(void);
IMPORT void shutdown_clock(void);

#endif /* _SYSDEPEND_TARGET_SYSDEPEND_ */