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
 *    tm_com.c
 *    T-Monitor Communication low-level device driver (No device)
 * 	This is a dummy program when there is no communication device.
 */
#include <stdio.h>

#include <tk/typedef.h>
#include "../../libtm.h"

#ifdef USE_COM_SIM_LINUX

EXPORT	void	tm_snd_dat( const UB* buf, INT size )
{
	while(size--) {
		putchar((int)*buf++);
	}
}


EXPORT	void	tm_rcv_dat( UB* buf, INT size )
{
	while(size--) {
		*buf++ = getchar();
	}
}


EXPORT	void	tm_com_init(void)
{
}

#endif /* USE_COM_NO_DEVICE */