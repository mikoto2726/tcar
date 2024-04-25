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
 *	profile.h
 *
 *	Service Profile (Simulator depended)
 */

#ifndef __SYS_DEPEND_PROFILE_H__
#define __SYS_DEPEND_PROFILE_H__


/*
 **** Target-depeneded profile (Simulator on Windows)
 */

#define TK_ALLOW_MISALIGN	(ALLOW_MISALIGN)	/* Memory misalign access is permitted */
#define TK_BIGENDIAN		(BIGENDIAN)		/* Is Big Endian (Must be defined) */

/*
 * Power management
 */
#define TK_SUPPORT_LOWPOWER	FALSE		/* Support of power management */

/*
 * Device Support
 */
#define TK_SUPPORT_IOPORT	TRUE		/* Support of I/O port access */

/*
 * Physical timer
 */
#define TK_SUPPORT_PTIMER	FALSE		/* Support of physical timer */
#define TK_MAX_PTIMER		0		/* Maximum number of physical timers. */

/*
 * FPU and co-processors
 */
#define TK_SUPPORT_FPU		FALSE		/* Support of FPU */
#define TK_SUPPORT_COP0		FALSE		/* Support of co-processor-0 */
#define TK_SUPPORT_COP1		FALSE		/* Support of co-processor-1 */
#define TK_SUPPORT_COP2		FALSE		/* Support of co-processor-2 */
#define TK_SUPPORT_COP3		FALSE		/* Support of co-processor-3 */

/*
 * low-level function
 */
#define TK_SUPPORT_REGOPS	FALSE		/* Support of get/set register operation */
#define TK_SUPPORT_ASM		FALSE		/* Support of assembly language function entry/exit */

/*
 * Interrupt
 */
#define TK_SUPPORT_INTCTRL	TRUE		/* Support of interrupt controller management. */
#define TK_HAS_ENAINTLEVEL	TRUE 		/* Can specify interrupt priority level */
#define TK_SUPPORT_CPUINTLEVEL	FALSE		/* Support of get/set of CPU interrupt mask level */
#define TK_SUPPORT_CTRLINTLEVEL	FALSE		/* Support of get/set of interrupt controller interrupt mask level */
#define TK_SUPPORT_INTMODE	TRUE		/* Supoprt of interrupt mode setting */

/*
 * Cache control
 */
#define TK_SUPPORT_CACHECTRL	FALSE			/* support of cache control */
#define TK_SUPPORT_WBCACHE	FALSE			/* Support of write-back cache */
#define TK_SUPPORT_WTCACHE	FALSE			/* Support of write-through cache */

/*
 * Real memory protection level of TA_RNGn (0..3)
 */
#define TK_MEM_RNG0		0
#define TK_MEM_RNG1		0
#define TK_MEM_RNG2		0
#define TK_MEM_RNG3		0

/*
 * Device Support
 */
#define TK_SUPPORT_MICROWAIT	TRUE			/* Support of micro wait */

#endif /* __SYS_DEPEND_PROFILE_H__ */
