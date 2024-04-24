﻿/*
 *----------------------------------------------------------------------
 *    Device Driver for micro T-Kernel for μT-Kernel 3.0
 *
 *    Copyright (C) 2020 by Ken Sakamura.
 *    This software is distributed under the T-License 2.2.
 *----------------------------------------------------------------------
 *
 *    Released by TRON Forum(http://www.tron.org) at 2020/12/09.
 *
 *----------------------------------------------------------------------
 */


/*
 *	device.c
 *	Device driver common definition
*/

#ifndef	__DEVINC_DEVICE_H__
#define	__DEVINC_DEVICE_H__

#include "../config/devconf.h"

#if DEVCNF_DEV_SER
#include "dev_ser.h"
#endif  /* DEVCNF_DEV_SER */

#if DEVCNF_DEV_ADC
#include "dev_adc.h"
#endif  /* DEVCNF_DEV_ADC */

#if DEVCNF_DEV_IIC
#include "dev_i2c.h"
#endif  /* DEVCNF_DEV_IIC */

#if DEVCNF_DEV_PMD
#include "dev_pmd.h"
#endif  /* DEVCNF_DEV_PMD */

#endif		/* __DEVINC_DEVICE_H__ */
