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
 *	hw_setting.c (Simulator)
 *	startup / shoutdown processing for hardware
 */
#include <simconf.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#define PROHIBIT_DEF_SIZE_T
#include "kernel.h"
#include <tm/tmonitor.h>
#include "sysdepend.h"

#if USE_IO_SIMULATION

/* ------------------------------------------------------
 * SFR simulation
 */
#define SFR_ADDR	0x40000000
#define SFR_SIZE	0x02000000

#define POWERSTS	((_UW*)0x41FE8FF0)	/* Fake SFR to inform power status */

LOCAL int   sfr_fd;

LOCAL void start_sfr_sim(void)
{
        int	ret;
	void*	p;

	/* Open SFR Emulator (Address : 0x40000000-0x41FFFFFF) */
	sfr_fd = open("/tmp/sim-linux-sfr", O_CREAT|O_RDWR, 0666);
	if ( sfr_fd < 0 ) {
		goto err_ret0;
	}

	ret = ftruncate(sfr_fd, SFR_SIZE);
        if ( ret != 0 ) {
		goto err_ret1;
        }

	p = mmap((void*)SFR_ADDR, SFR_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, sfr_fd, 0);
	if ( p != (void*)SFR_ADDR ) {
		goto err_ret2;
	}
	if ( p == NULL ) {
		goto err_ret1;
	}

	/* Initialize register values */
	memset((void*)SFR_ADDR, 0, SFR_SIZE);
	*POWERSTS = 1;
	*(_UW*)PORTA_DATA = (1<<3);
	*(_UW*)PORTE_DATA = (1<<7);

	return;

 err_ret2:
	munmap(p, SFR_SIZE);
 err_ret1:
	close(sfr_fd);
 err_ret0:
	sfr_fd = -1;
	fprintf(stderr, "WARNING: failed to map SFR registers at address %p\n", (void*)SFR_ADDR);
	return;
}

LOCAL void stop_sfr_sim( void )
{
	disint();
	if ( sfr_fd >= 0 ) {
		*POWERSTS = 0;
		munmap((void*)SFR_ADDR, SFR_SIZE);
		close(sfr_fd);
	}
}

#endif	/* USE_IO_SIMULATION */

/* ------------------------------------------------------
 * Startup Device
 */
EXPORT void knl_startup_hw(void)
{
#if USE_IO_SIMULATION
	start_sfr_sim();
#endif
}

#if USE_SHUTDOWN
/* ------------------------------------------------------
 * Shutdown device
 */
EXPORT void knl_shutdown_hw( void )
{
#if USE_IO_SIMULATION
	stop_sfr_sim();
#endif
	exit(0);	 
}
#endif /* USE_SHUTDOWN */


/* ------------------------------------------------------
 * Re-start device
 *	mode = -1		reset and re-start	(Reset -> Boot -> Start)
 *	mode = -2		fast re-start		(Start)
 *	mode = -3		Normal re-start		(Boot -> Start)
 */
EXPORT ER knl_restart_hw( W mode )
{
	switch(mode) {
	case -1: /* Reset and re-start */
		SYSTEM_MESSAGE("\n<< SYSTEM RESET & RESTART >>\n");
		return E_NOSPT;
	case -2: /* fast re-start */
		SYSTEM_MESSAGE("\n<< SYSTEM FAST RESTART >>\n");
		return E_NOSPT;
	case -3: /* Normal re-start */
		SYSTEM_MESSAGE("\n<< SYSTEM RESTART >>\n");
		return E_NOSPT;
	default:
		return E_PAR;
	}
}

#endif /* SIM_LINUX */
