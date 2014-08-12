/*
 * spi_dac8043.c
 *
 *  Created on: 2014年8月12日
 *      Author: beny
 */
#include "msp430.h"
#include "spi_dac8043.h"
#define LOD_H						(P4OUT |= BIT0)
#define LOD_L							(P4OUT &= ~BIT0)
void spi_init()
{
	P4SEL |= BIT1+BIT2+BIT3;//Pin select
	P4DIR |= BIT0;
	UCB1CTL1 |= UCSWRST;                      // **Put state machine in reset**
	UCB1CTL0 |= UCMST+UCSYNC+UCCKPL+UCMSB;    // 3-pin, 8-bit SPI master
	                                            // Clock polarity high, MSB
	UCB1CTL1 |= UCSSEL_2;                     // SMCLK
	UCB1BR0 = 0;                           // /0
	UCB1BR1 = 0;                              //
	  //UCB1MCTL = 0;                             // No modulation
	UCB1CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
}

void spi_write(unsigned int spi_data)
{
	while(!(UCB1IFG & UCTXIFG));
	UCB1TXBUF = spi_data >> 8;
	while(!(UCB1IFG & UCTXIFG));
	UCB1TXBUF = spi_data &0xff ;
}


