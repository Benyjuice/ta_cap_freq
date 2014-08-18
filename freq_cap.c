/*
 * freq_cap.c
 *
 *  Created on: 2014年8月12日
 *      Author: beny
 */
#include "msp430.h"
#include "freq_cap.h"
#include "spi_dac8043.h"
#include "MatrixKey.h"
#include "lcd12864.h"
int freq;
int cap_count;
void freq_cap_init()
{
	/*
	 * Timer_A2 init
	 */
	P2SEL |= BIT3	;																		// Pin Selected,P2.3 TA2.0
	TA2CCTL0 =  CM_1 +  SCS +  CAP;										//Capture on raising edge,Synchronous capture,
	TA2CTL = TASSEL_2 + MC_2  + TACLR;								//SMCLK,Continuous mode,clear
																										//Capture mode,Interrupt Enable,CCIxA
	TA0CCTL0 = CCIE;                          // CCR0 interrupt enabled
	TA0CCR0 = 6382-1;						//if 100ms capture no signal ,reset freq_mea mode
	TA0CTL = TASSEL_1 + MC_1 + ID_3 + TACLR;         // ACLK, upmode, clear TAR
	_EINT();																			//Enable CPU Interrput

}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER2_A0_VECTOR
__interrupt void TIMER2_A0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER2_A0_VECTOR))) TIMER2_A0_ISR (void)
#else
#error Compiler not supported!
#endif
{
	cap_count ++;
	if (TA2CCTL0 & COV)	 { //if overfllow
		TA2CCTL0&= ~COV;
	}
}

extern char Cap_Flg;
/*
 * if TA1 capture no signal ,TIMER0 interrupt after 0.5 seconds and clear freq
 */
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) TIMER0_A0_ISR (void)
#else
#error Compiler not supported!
#endif
{
	TA2CCTL0 &= ~ CCIE;
	 freq = cap_count*10.00;
	 cap_count = 0;
	 TA2CCTL0 &= ~COV;
	 TA0CTL |= TACLR;
	 TA2CCTL0 |= CCIE;
	 if(Cap_Flg)
		 freq = 4603+(int)(ADC12MEM0 & 0xff);
	 else
		 freq = 0;
	 	unsigned char fr[7]={'\0'};
	 	fr[0] = (int)freq/1000+'0';
	 	fr[1] = ((int)freq%1000)/100+'0';
	 	fr[2] = ((int)freq%100)/10+'0';
	 	fr[3] = ((int)freq%10)+'0';
	 	fr[4]='H';
	 	fr[5]='z';
	 	write_zi(0,3,fr);
	  //spi_write(3142);

}
