/*
 *----------------------------------------------------------------------
 *    Device Driver for micro T-Kernel for μT-Kernel 3.0
 *
 *    Copyright (C) 2020 by Ken Sakamura.
 *    This software is distributed under the T-License 2.2.
 *----------------------------------------------------------------------
 *
 *    Released by TRON Forum(http://www.tron.org) at 2020/10/21.
 *
 *----------------------------------------------------------------------
 */


#include <sys/machine.h>
#ifdef SIM_LINUX
#include "../../../config/devconf.h"
#if DEVCNF_DEV_IIC
/*
 *	i2c_sim.c
  *	I2C device driver
 *	System-dependent definition for Simulator
 */
#include <tk/tkernel.h>
#include <tm/tmonitor.h>
#include <tstdlib.h>
#include "../../i2c.h"

/*----------------------------------------------------------------------
 * Hedlight control
 */
#define	I2C1_REG1		(0x400E0100UL)
#define	I2C1_REG2		(0x400E0104UL)

#define I2C_ADDR_HL	0x18

#define	HLREG_NUM	18
#define HLREG_MODE1	0x00
#define HLREG_MODE2	0x01
#define HLREG_LEDOUT0	0x0C
#define HLREG_LEDOUT1	0x0D

#define HLREG_PWM0	0x02
#define HLREG_PWM1	0x03
#define HLREG_PWM2	0x04
#define HLREG_PWM4	0x06
#define HLREG_PWM5	0x07
#define HLREG_PWM6	0x08

LOCAL UB hl_reg[HLREG_NUM];

LOCAL UB get_color(UB r, UB g, UB b)
{
	UB color = 0;

	if(r) color |= (1<<2);
	if(g) color |= (1<<1);
	if(b) color |= (1<<0);

	return color;
}

LOCAL ER write_hl(UW cnt, UB *p)
{
	UB	color;

	if(cnt != 2 || *p > HLREG_NUM) return E_IO;
	hl_reg[*p] = *(p+1);

	if((hl_reg[HLREG_MODE1] == 0x00) && (hl_reg[HLREG_MODE2] == 0x05)) {
		if(hl_reg[HLREG_LEDOUT0] == 0x2A) {
			color = get_color(hl_reg[HLREG_PWM0],hl_reg[HLREG_PWM1],hl_reg[HLREG_PWM2] );
			*(UW*)I2C1_REG1 = color;
			tm_printf((UB*)"Right HL %d\n", color);
		}
		if(hl_reg[HLREG_LEDOUT1] == 0x2A) {
			color = get_color(hl_reg[HLREG_PWM4],hl_reg[HLREG_PWM5],hl_reg[HLREG_PWM6] );			
			*(UW*)I2C1_REG2 = color;
			tm_printf((UB*)"left HL %d\n", color);
		}
	}

	return E_OK;
}

/*----------------------------------------------------------------------
 * Color sendor control
 */
#define I2C_ADDR_CS0	0x38
#define I2C_ADDR_CS1	0x39
#define I2C_ADDR_CS2	0x3C
#define I2C_ADDR_CS3	0x3D

#define	CSCH_NUM	4
#define	CSREG_NUM	2

#define CSNS_DATA	55

LOCAL UB cs_reg[CSCH_NUM][CSREG_NUM];

LOCAL ER write_cs(UB sadr, UW cnt, UB *p)
{
	INT	ch, i;

	ch = sadr - I2C_ADDR_CS0;
	if(ch > 2) ch -= 2;

	if(cnt != 2) return E_IO;

	if(*p == 0x40) i = 0;
	else if(*p == 0x42) i = 1;
	else return E_IO;

	cs_reg[ch][i] = *(p+1);

	return E_OK;
}

LOCAL ER exec_cs(UB sadr, T_I2C_EXEC *p_exec)
{
	const UB cdata[] = {20,0,19,0,21,0,100,0};

	INT	ch, radr;
	UB	*wp, *sp;

	ch = sadr - I2C_ADDR_CS0;
	if(ch > 2) ch -= 2;

	if(p_exec->snd_size != 1) return E_IO;
	radr = *(p_exec->snd_data);

	if((radr == 0x42) && (p_exec->rcv_size == 1)) {
		*(p_exec->rcv_data) = cs_reg[ch][1] | (1<<7);
	} else if((radr >= 0x50)&&(radr <= 0x57)) {
		if(p_exec->rcv_size > (0x57-radr+1)) return E_IO;

		wp = p_exec->rcv_data;
		sp = &cdata[radr-0x50];
		for(INT i = 0; i < p_exec->rcv_size; i++) {
			*wp++ = *sp++;
		}
	} else {
		return E_IO;
	}
	return E_OK;
}

/*----------------------------------------------------------------------
 * Low level device control
 */
EXPORT W dev_i2c_llctl( UW unit, INT cmd, UW p1, UW p2, UW *pp)
{
	UW	sadr;
	ER	err	= E_OK;

	if(unit != 1) return E_IO;

	switch(cmd) {
	case LLD_I2C_OPEN:
		break;

	case LLD_I2C_CLOSE:
		break;

	case LLD_I2C_READ:
		err = E_IO;
		break;

	case LLD_I2C_WRITE:
		if(p1 == I2C_ADDR_HL) err = write_hl(p2, (UB*)pp);
		else if((p1 == I2C_ADDR_CS0) || (p1 == I2C_ADDR_CS1)
			|| (p1 == I2C_ADDR_CS2) || (p1 == I2C_ADDR_CS3)) err = write_cs( p1, p2, (UB*)pp);
		else err = E_IO;
		break;

	case LLD_I2C_EXEC:
		sadr = ((T_I2C_EXEC*)pp)->sadr;
		if((sadr == I2C_ADDR_CS0) || (sadr == I2C_ADDR_CS1)
			|| (sadr == I2C_ADDR_CS2) || (sadr == I2C_ADDR_CS3)) exec_cs( sadr, (T_I2C_EXEC*)pp);
		else err = E_IO;
		break;
	}
	
	return (W)err;
}

/*----------------------------------------------------------------------
 * Device initialization
 */
EXPORT ER dev_i2c_llinit( T_I2C_DCB *p_dcb)
{
	knl_memset((void*)hl_reg, 0, sizeof(hl_reg));
	knl_memset((void*)cs_reg, 0, sizeof(cs_reg));

	return E_OK;
}

#endif		/* DEVCNF_DEV_IIC */
#endif		/* SIM_LINUX */