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
#define CLK_H			P4OUT |= BIT3
#define CLK_L			P4OUT &= ~BIT3
#define SRI_H			P4OUT |= BIT2
#define SRI_L			P4OUT &= ~BIT2

/*
 * P4.3 -- CLK
 * P4.2 -- SRI
 * P4.0 -- LD
 *
 */




void spi_init()
{
	//P4SEL |= BIT1+BIT2+BIT3;//Pin select
	P4DIR |= BIT0+BIT1 +BIT2 +BIT3;
	/*UCB1CTL1 |= UCSWRST;                      // **Put state machine in reset**
	UCB1CTL0 |= UCMST+UCSYNC+UCCKPH+UCMSB;// + UC7BIT;    // 3-pin, 8-bit SPI master
	                                            // Clock polarity high, MSB
	UCB1CTL1 |= UCSSEL_2;                     // SMCLK
	UCB1BR0 = 4-1;                           // /0
	UCB1BR1 = 0;                              //
	  //UCB1MCTL = 0;                             // No modulation
	UCB1CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**/
}

void spi_write(unsigned int spi_data)
{
	int temp = spi_data;
	int i;
	for (i = 0;i<12;i++) {
		if(temp & 0x800)
			SRI_H;
		else
			SRI_L;
		_NOP();
		_NOP();
		_NOP();
		CLK_H;
		_NOP();
		_NOP();
		_NOP();
		CLK_L;
		_NOP();
		_NOP();
		_NOP();
		temp <<= 1;
	}
	LOD_L;
	LOD_H;

	/*LOD_H;
	while(!(UCB1IFG & UCTXIFG));
	UCB1TXBUF = spi_data >> 8;
	while(!(UCB1IFG & UCTXIFG));
	UCB1TXBUF = spi_data &0xff ;
	while(!(UCB1IFG & UCTXIFG));
	LOD_L;
	_NOP();
	_NOP();
	LOD_H;*/

}


