/*
 * freq_cap.c
 *
 *  Created on: 2014年8月12日
 *      Author: beny
 */
#include "msp430.h"
#include "freq_cap.h"
#include "spi_dac8043.h"

static unsigned int freq_cap_tar[F_MEAS_NUM]={0};
static unsigned int freq_cap_tar_index=0;
float freq;
void freq_cap_init()
{
	/*
	 * Timer_A2 init
	 */
	P2SEL |= BIT3	;																		// Pin Selected,P2.3 TA2.0
	TA2CTL = TASSEL_2 + MC_2  + TACLR;					//SMCLK,Continuous mode,clear
	TA2CCTL0 =  CM_1 +  SCS +  CAP + CCIE;			//Capture on raising edge,Synchronous capture,
																							//Capture mode,Interrupt Enable,CCIxA
	TA0CCTL0 = CCIE;                          // CCR0 interrupt enabled
	TA0CCR0 = 32768-1;						//1s
	TA0CTL = TASSEL_1 + MC_1 + TACLR;         // ACLK, upmode, clear TAR
	_EINT();																			//Enable CPU Interrput

}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER2_A0_VECTOR
__interrupt void TIMER2_A0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER1_A0_VECTOR))) TIMER1_A0_ISR (void)
#else
#error Compiler not supported!
#endif
{
	if (!(TA2CCTL0 & COV))	 {
		freq_cap_tar[freq_cap_tar_index++]=TA2CCR0;
		if (freq_cap_tar_index >= F_MEAS_NUM) {
			freq_cap_tar_index = 0;
			int i;
			unsigned int temp=0;
			unsigned long sum=0;
			for (i=F_MEAS_NUM-1;i>0;i--) {
				if (freq_cap_tar[i-1] >= freq_cap_tar[i])
					temp = freq_cap_tar[i]+(0xffff - freq_cap_tar[i-1]);
				else
					temp = freq_cap_tar[i] - freq_cap_tar[i-1];
					sum += temp;
			}
			sum = sum/(F_MEAS_NUM-1);
			freq = F_MEAS_FREQ / sum;
		}
	}else	{																					//if overflow occures
		freq_cap_tar_index=0;
		TA1CCTL0 &= ~COV;
	}
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) TIMER0_A0_ISR (void)
#else
#error Compiler not supported!
#endif
{
	 freq_cap_tar_index=0;
	 spi_write(1024);
}
