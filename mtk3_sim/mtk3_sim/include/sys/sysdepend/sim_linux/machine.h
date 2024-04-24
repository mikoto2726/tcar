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
 *	machine.h
 *
 *	Machine type definition (Simulator depended)
 */

#ifndef __SYS_SYSDEPEND_MACHINE_H__
#define __SYS_SYSDEPEND_MACHINE_H__

/*
 * [TYPE]_[CPU]		TARGET SYSTEM
 * CPU_xxxx		CPU type
 * CPU_CORE_xxx		CPU core type
 */

/* ----- Simulator definition ----- */

#define SIM_LINUX			1				/* Target system : Simulator on Windows */
#define CPU_SIM			1				/* Target CPU : Simulator */

#define TARGET_DIR		sim_linux				/* Sysdepend-Directory name */

#define ALLOW_MISALIGN		0
#define INT_BITWIDTH		32

#define BIGENDIAN		0	/* Default (Little Endian) */

#endif /* __SYS_SYSDEPEND_MACHINE_H__ */
