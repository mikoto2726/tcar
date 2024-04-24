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
 *	sys_timer.h (Simulator)
 *	Hardware-Dependent System Timer Processing
 */

#ifndef _SYSDEPEND_TARGET_SYSTIMER_
#define _SYSDEPEND_TARGET_SYSTIMER_

/*
 * Timer start processing
 *	Initialize the timer and start the periodical timer interrupt.
 */
IMPORT void knl_start_hw_timer( void );

/*
 * Clear timer interrupt
 *	Clear the timer interrupt request. Depending on the type of
 *	hardware, there are two timings for clearing: at the beginning
 *	and the end of the interrupt handler.
 *	'clear_hw_timer_interrupt()' is called at the beginning of the
 *	timer interrupt handler.
 *	'end_of_hw_timer_interrupt()' is called at the end of the timer
 *	interrupt handler.
 *	Use either or both according to hardware.
 */
Inline void knl_clear_hw_timer_interrupt( void )
{
	/* do nothing */
}

Inline void knl_end_of_hw_timer_interrupt( void )
{
	/* do nothing */
}

/*
 * Timer stop processing
 *	Stop the timer operation.
 *	Called when system stops.
 */
IMPORT void knl_terminate_hw_timer(void);

/*
 * Get processing time from the previous timer interrupt to the
 * current (nanosecond)
 *	Consider the possibility that the timer interrupt occurred
 *	during the interrupt disable and calculate the processing time
 *	within the following
 *	range: 0 <= Processing time < TIMER_PERIOD * 2
 */
Inline UW knl_get_hw_timer_nsec( void )
{
	return 0;
}

/* 
 * Pause system timer
 */
IMPORT void knl_pause_systim(void);

/* 
 * Resume system timer
 */
IMPORT void knl_resume_systim(void);

/*
 *System - timer Interrupt handler
 */
IMPORT void knl_systim_inthdr(int signum);

#endif /* _SYSDEPEND_TARGET_SYSTIMER_ */