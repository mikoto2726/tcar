/*
 *----------------------------------------------------------------------
 *    micro T-Kernel 3.00.03.B0
 *
 *    Copyright (C) 2006-2020 by Ken Sakamura.
 *    This software is distributed under the T-License 2.2.
 *----------------------------------------------------------------------
 *
 *    Released by TRON Forum(http://www.tron.org) at 2020/12/09.
 *
 *----------------------------------------------------------------------
 */

/*
 *	sys_msg.h (STM32L4 IoT-Engine)
 *	Hardware-Dependent System message
 */

#ifndef _SYSDEPEND_TARGET_SYSMSG_
#define _SYSDEPEND_TARGET_SYSMSG_

#include <tm/tmonitor.h>

#if (USE_SYSTEM_MESSAGE && USE_TMONITOR)
#define SYSTEM_MESSAGE(s)	tm_putstring((UB*)s)
#else
#define SYSTEM_MESSAGE(s)
#endif /* USE_SYSTEM_MESSAGE && USE_TMONITOR */

#endif /* _SYSDEPEND_TARGET_SYSMSG_ */