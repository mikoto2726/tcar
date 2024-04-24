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
 *	sysdef.h
 *
 *	System dependencies definition (Simulator depended)
 *	Included also from assembler program.
 */

#ifndef __SYS_SYSDEF_DEPEND_H__
#define __SYS_SYSDEF_DEPEND_H__

#define POINTER_SIZE_64BIT

#ifndef offsetof
#define offsetof(type, member) ((D)(&((type *)0)->member))
#endif

/* ------------------------------------------------------------------------ */
/*
 * Clock Control
 */
/*
 * Settable interval range (millisecond)
 */
#define MIN_TIMER_PERIOD	1
#define MAX_TIMER_PERIOD	50

/*----------------------------------------------------------*/
/*
 * Interrupt Control
 */
/*
 * Number of Interrupt vectors
 */
#define N_INTVEC		32	/* Number of Interrupt vectors */
/*
 * Time-event handler interrupt level
 */
#define TIMER_INTLEVEL		0

/* ------------------------------------------------------------------------ */
/*
 * Physical timer
 */
#define	CPU_HAS_PTMR	(0)

/* ------------------------------------------------------------------------ */
/*
 * Coprocessor
 */
#define CPU_HAS_FPU			0
#define CPU_HAS_DPS			0

/*
 *  Number of coprocessors to use. 
 *    There is no coprocessor for this microcomputer.
 */
#define NUM_COPROCESSOR		0

/* ------------------------------------------------------------------------ */
/*
 * Maximum value of Power-saving mode switching prohibition request.
 * Use in tk_set_pow API.
 */
#define LOWPOW_LIMIT	0x7fff		/* Maximum number for disabling */

/* ------------------------------------------------------------------------ */
/*
 * Definition of minimum system stack size
 *	Minimum system stack size when setting the system stack size
 *	per task by 'tk_cre_tsk().'
 *  this size must be larger than the size of SStackFrame
 */
#define MIN_SYS_STACK_SIZE	128

/*
 * Default task system stack 
 */

#define DEFAULT_SYS_STKSZ	MIN_SYS_STACK_SIZE


/* ------------------------------------------------------------------------ */
/*
 * irtual device address definition
 */
#define PORTA_DATA 	0x400C0000
#define PORTA_CR	0x400C0004
#define	PORTA_FR3	0x400C0010
#define PORTA_IE	0x400C0038

#define PORTE_DATA	0x400C0400
#define PORTE_CR	0x400C0404
#define	PORTE_FR3	0x400C0410
#define PORTE_IE	0x400C0438

#define PORTF_DATA	0x400C0500
#define PORTF_CR	0x400C0504
#define PORTF_IE	0x400C0538

#endif /* __TK_SYSDEF_DEPEND_H__ */
