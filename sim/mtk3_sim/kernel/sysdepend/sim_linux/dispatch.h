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
 *	cpu_status.h (Simulator)
 *	CPU-Dependent Task Start Processing
 */

#ifndef _SYSDEPEND_TARGET_DISPATCH_
#define _SYSDEPEND_TARGET_DISPATCH_

typedef struct {
	jmp_buf		env;
	jmp_buf		env0;
	UW		*sp_e;
} T_SIM_TSKENV;

#define STACK_MAGIC	(1234)		/* Magic number for stack overflow check */

#endif  /* _SYSDEPEND_TARGET_DISPATCH_ */