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
 *    tm_com.c
 *    T-Monitor Communication low-level device driver (STM32L4 IoT-Engine)
 */

#include <tk/typedef.h>
#include <sys/sysdef.h>
#include "../../libtm.h"

#ifdef USE_COM_IOTE_STM32

/* UART register definition (Use USART2) */
#define	UART_BASE		(0x40004400UL)		/* USART2 register base address */

#define UART_CR1	(*(_UW*)(UART_BASE+0x0000))	/* Control register.1 */
#define UART_CR2	(*(_UW*)(UART_BASE+0x0004))	/* Control register.2 */
#define UART_CR3	(*(_UW*)(UART_BASE+0x0008))	/* Control register.3 */
#define UART_BRR	(*(_UW*)(UART_BASE+0x000C))	/* Baud Rate register */
#define UART_GTPR	(*(_UW*)(UART_BASE+0x0010))	/* GUARD TIME and PRESCALER register */
#define UART_RTOR	(*(_UW*)(UART_BASE+0x0014))	/* RECEIVER TIMEOUT register */
#define UART_RQR	(*(_UW*)(UART_BASE+0x0018))	/* REQUEST register */
#define UART_ISR	(*(_UW*)(UART_BASE+0x001C))	/* INTERRUPT and STATUS register */
#define UART_ICR	(*(_UW*)(UART_BASE+0x0020))	/* INTERRUPT FLAG CLEAR register */
#define UART_RDR	(*(_UW*)(UART_BASE+0x0024))	/* RECEIVE DATA register */
#define UART_TDR	(*(_UW*)(UART_BASE+0x0028))	/* TRANSMIT DATA register */

#define CR1_UE		(0x00000001)			/* Enable UART */
#define CR1_RE		(0x00000004)			/* Enable reception */
#define CR1_TE		(0x00000008)			/* Enable sending */

#define ISR_TXE		(0x00000080)			/* Transmit data register empty */
#define ISR_TC		(0x00000040)			/* Transmission complete */
#define ISR_RXNE	(0x00000020)			/* Read data register not empty */

/* Communication speed */
#define UART_BAUD	(115200)			/* 115200 bps */

EXPORT	void	tm_snd_dat( const UB* buf, INT size )
{
	UB	*b;

	for( b = (UB *)buf; size > 0; size--, b++ ){
		while ((UART_ISR & ISR_TXE) == 0 );
		UART_TDR = *b;
		while ((UART_ISR & ISR_TC) == 0 );
	}
}


EXPORT	void	tm_rcv_dat( UB* buf, INT size )
{
	for( ; size > 0; size--, buf++ ){
		while ( (UART_ISR & ISR_RXNE) == 0 );
		*buf = UART_RDR & 0xff;
	}
}


EXPORT	void	tm_com_init(void)
{
	/* Initialize serial communication. Disable all interrupt. */
	UART_CR1 = 0;		/* 8bit, Non parity (Reset value) */
	UART_CR2 = 0;		/* Stop bit 1 (Reset value) */
	UART_CR3 = 0;		/* No hard flow control (Reset value) */

	/* Set baud rate */
	UART_BRR = ((TMCLK*1000*1000) + UART_BAUD/2)/UART_BAUD;

	UART_CR1 = CR1_UE | CR1_RE |CR1_TE;	/* Start UART */
}

#endif /* USE_COM_IOTE_STM32 */