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
 *	syslib.h
 *
 *	micro T-Kernel System Library  (Simulator depended)
 */

#ifndef __TK_SYSLIB_DEPEND_H__
#define __TK_SYSLIB_DEPEND_H__

/*----------------------------------------------------------------------*/
/*
 * CPU interrupt control for Simulator.
 * 
 */
IMPORT void enaint(UW intsts);		/* Enable interrupt */
IMPORT UW disint(void);			/* Disable interrupt */
IMPORT UW is_disint(void);		/* Is interrupt disabled? */

#define DI(intsts)		( (intsts) = disint() )
#define EI(intsts)		( enaint(intsts) )
#define isDI(intsts)		( (intsts) != 0 )

#define INTLEVEL_DI		(1)
#define INTLEVEL_EI		(0)

IMPORT void knl_pause_systim(void);
IMPORT void knl_resume_systim(void);

#define IM_LEVEL	0x0002		/* Level trigger */
#define IM_EDGE		0x0000		/* Edge trigger */
#define IM_HI		0x0000		/* H level/Interrupt at rising edge */
#define IM_LOW		0x0001		/* L level/Interrupt at falling edge */

#endif /* __TK_SYSLIB_DEPEND_H__ */
