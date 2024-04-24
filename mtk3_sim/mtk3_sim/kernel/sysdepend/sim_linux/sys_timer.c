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
 *	sys_timer.c (Simulator)
 *	Hardware-Dependent System Timer Processing
 */
#include <simconf.h>

#include <unistd.h>
#include <signal.h>
#include <sys/time.h>

#define PROHIBIT_DEF_SIZE_T
#include "kernel.h"
#include "sysdepend.h"
#include "sys_timer.h"

#include <tk/syslib.h>

const LOCAL struct itimerval systim_run = {
	{ 0, TIMER_PERIOD * 1000 },	// it_interval
	{ 0, TIMER_PERIOD * 1000 }	// it_value
};

const LOCAL struct itimerval systim_stop = {{0, 0}, {0, 0}};

#define SYSTIM_STOP 0
#define SYSTIM_RUN 1
EXPORT volatile sig_atomic_t systim_stat = SYSTIM_STOP;

/*
 * Timer start processing
 *	Initialize the timer and start the periodical timer interrupt.
 */
EXPORT void knl_start_hw_timer(void)
{
	knl_define_inthdr(SIGALRM, TA_HLNG, knl_timer_handler);
	EnableInt(SIGALRM, 0);

	systim_stat = SYSTIM_RUN;
	setitimer(ITIMER_REAL, &systim_run, NULL);
}

/*
 * Timer stop processing
 *	Stop the timer operation.
 *	Called when system stops.
 */
EXPORT void knl_terminate_hw_timer( void )
{
	systim_stat = SYSTIM_STOP;
	setitimer(ITIMER_REAL, &systim_stop, NULL);
}

/* 
 * Pause system timer
 */
EXPORT void knl_pause_systim(void)
{
	setitimer(ITIMER_REAL, &systim_stop, NULL);
}

/* 
 * Resume system timer
 */
EXPORT void knl_resume_systim(void)
{
	setitimer(ITIMER_REAL, &systim_run, NULL);
}

#endif /* SIM_LINUX */
