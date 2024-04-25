/*
 * uT-Kernel PDM(Programmable Motor Driver) Device for TX03-M367
 * Copyright (c) 2019 by INIAD
 */

#include <sys/machine.h>
#ifdef SIM_LINUX
#include "../config/devconf.h"
#if DEVCNF_DEV_PMD
/*
 *	i2c_sim.c
  *	I2C device driver
 *	System-dependent definition for Simulator
 */
#include <tk/tkernel.h>
#include <tm/tmonitor.h>
#include <tstdlib.h>

#include "../common/drvif/msdrvif.h"
#include "../include/dev_pmd.h"

/*-------------------------------------------------------
 * PWM global data
 *-------------------------------------------------------
 */
#define		PMD_MAX_PERIOD	14563

typedef struct {
	ID		devid;		/* Device ID */
	UW		en;		/* PWM enable */
	UW		period;		/* PWM period */
	UW		pulse0;		/* pulse width 0 */
	UW		pulse1;		/* pulse width 1 */
	UW		pulse2;		/* pulse width 2 */
} T_PMDCB;

LOCAL T_PMDCB	pmdcb;

/*-------------------------------------------------------
 * mSDI I/F Function
 *-------------------------------------------------------
 */
LOCAL ER dev_pmd_openfn( ID devid, UINT omode, T_MSDI *msdi)
{
	return E_OK;
}

LOCAL ER dev_pmd_closefn( ID devid, UINT option, T_MSDI *msdi)
{
	return E_OK;
}


LOCAL INT dev_pmd_readfn( T_DEVREQ *req, T_MSDI *p_msdi)
{
	UW	data;
	ER	err = E_OK;

	if(req->size != 1) return E_PAR;

	switch(req->start) {
	case PMD_DATA_EN:
		data = pmdcb.en;
		break;
	case PMD_DATA_PERIOD:
		data = pmdcb.period;
		break;
	case PMD_DATA_PULSE0:
		data = pmdcb.pulse0;
		break;
	case PMD_DATA_PULSE1:
		data = pmdcb.pulse1;
		break;
	case PMD_DATA_PULSE2:
		data = pmdcb.pulse2;
		break;
	default:
		err = E_PAR;
	}
	if(err == E_OK) {
		*(UW*)(req->buf) = data;
	}

	return err;
}

/*----------------------------------------------------------------------
 * Event Device
 */
ER dev_pmd_eventfn( INT evttyp, void *evtinf, T_MSDI *msdi)
{
	return E_NOSPT;
}

LOCAL INT dev_pmd_writefn( T_DEVREQ *req, T_MSDI *p_msdi)
{
	ER	err = E_OK;
	UW	data;

	if(req->size != 1) return E_PAR;

	data = *(UW*)(req->buf);

	/* 範囲チェック */
	switch(req->size) {
	case PMD_DATA_EN:
		if(data > 1) err = E_PAR;
		break;
	case PMD_DATA_PERIOD:
		if( data <= 0 || data > PMD_MAX_PERIOD) err = E_PAR;
		break;
	case PMD_DATA_PULSE0:
	case PMD_DATA_PULSE1:
	case PMD_DATA_PULSE2:
		if( data <= 0 || data >= pmdcb.period) err = E_PAR;
		break;
	default:
		err = E_PAR;
	}
	if(err != E_OK ) return err;

	switch(req->start) {
	case PMD_DATA_EN:
		pmdcb.en = data;
		break;
	case PMD_DATA_PERIOD:
		pmdcb.period = data;
		break;
	case PMD_DATA_PULSE0:
		pmdcb.pulse0 = data;
		break;
	case PMD_DATA_PULSE1:
		pmdcb.pulse1 = data;
		break;
	case PMD_DATA_PULSE2:
		pmdcb.pulse2 = data;
		break;
	default:
		err = E_PAR;
	}

	return err;
}


/*
 * PMD Device initialization and registration
 */
EXPORT ER dev_init_pmd( void )
{
	T_DMSDI		dmsdi;
	T_IDEV		idev;
	T_MSDI		*p_msdi;
	INT		i;
	ER		err;

	/* Device registration information */
	dmsdi.exinf	= &pmdcb;
	dmsdi.drvatr	= 0;			/* Driver attributes */
	dmsdi.devatr	= TDK_UNDEF;		/* Device attributes */
	dmsdi.nsub	= 0;			/* Number of subunits */
	dmsdi.blksz	= 1;			/* Unique data block size (-1 = unknown) */
	dmsdi.openfn	= dev_pmd_openfn;
	dmsdi.closefn	= dev_pmd_closefn;
	dmsdi.readfn	= dev_pmd_readfn;
	dmsdi.writefn	= dev_pmd_writefn;
	dmsdi.eventfn	= dev_pmd_eventfn;
	
	knl_strcpy((char*)dmsdi.devnm, "pmda");
	i = knl_strlen("pmda");
	dmsdi.devnm[i] = 0;

	err = msdi_def_dev( &dmsdi, &idev, &p_msdi);

	return err;
}

#endif		/* DEVCNF_DEV_PMD */
#endif		/* SIM_LINUX */