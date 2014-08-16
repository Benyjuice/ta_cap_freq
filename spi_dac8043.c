/*
 * spi_dac8043.c
 *
 *  Created on: 2014年8月12日
 *      Author: beny
 */
#include "msp430.h"
#include "spi_dac8043.h"
#define LOD1_H						(P4OUT |= BIT0)
#define LOD1_L							(P4OUT &= ~BIT0)
#define CLK1_H			P4OUT |= BIT3
#define CLK1_L			P4OUT &= ~BIT3
#define SRI1_H			P4OUT |= BIT2
#define SRI1_L			P4OUT &= ~BIT2

#define LOD2_H					(P2OUT |= BIT6)
#define LOD2_L 					(P2OUT &= ~BIT6)
#define CLK2_H					(P2OUT |= BIT4)
#define CLK2_L					(P2OUT &= ~BIT4)
#define SRI2_H					(P2OUT |= BIT5)
#define SRI2_L						(P2OUT &= ~BIT5)

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
	P2DIR |= BIT4+BIT5+BIT6;
	/*UCB1CTL1 |= UCSWRST;                      // **Put state machine in reset**
	UCB1CTL0 |= UCMST+UCSYNC+UCCKPH+UCMSB;// + UC7BIT;    // 3-pin, 8-bit SPI master
	                                            // Clock polarity high, MSB
	UCB1CTL1 |= UCSSEL_2;                     // SMCLK
	UCB1BR0 = 4-1;                           // /0
	UCB1BR1 = 0;                              //
	  //UCB1MCTL = 0;                             // No modulation
	UCB1CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**/
}

void spi1_write(unsigned int spi_data)
{
	int temp = spi_data;
	int i;
	for (i = 0;i<12;i++) {
		if(temp & 0x800)
			SRI1_H;
		else
			SRI1_L;
		_NOP();
		_NOP();
		_NOP();
		CLK1_H;
		_NOP();
		_NOP();
		_NOP();
		CLK1_L;
		_NOP();
		_NOP();
		_NOP();
		temp <<= 1;
	}
	LOD1_L;
	LOD1_H;
}

void spi2_write(unsigned int spi_data)
{
	int temp = spi_data;
	int i;
	for (i = 0;i<12;i++) {
		if(temp & 0x800)
			SRI2_H;
		else
			SRI2_L;
		CLK2_H;
		CLK2_L;
		temp <<= 1;
	}
	LOD2_L;
	LOD2_H;
}


