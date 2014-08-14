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
	TA2CCTL0 =  CM_1 +  SCS +  CAP + CCIE;			//Capture on raising edge,Synchronous capture,
	TA2CTL = TASSEL_2 + MC_2  + TACLR;					//SMCLK,Continuous mode,clear
																							//Capture mode,Interrupt Enable,CCIxA
	TA0CCTL0 = CCIE;                          // CCR0 interrupt enabled
	TA0CCR0 = 16384-1;						//if 0.5s capture no signal ,reset freq_mea mode
	TA0CTL = TASSEL_1 + MC_1 + TACLR;         // ACLK, upmode, clear TAR
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
	TA0CTL |= TACLR;							//Singnal captured,TA2 Clear TA0
	if (!(TA2CCTL1 & COV))	 {
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
		TA2CCTL0&= ~COV;
	}
}

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
	 freq_cap_tar_index=0;
	 freq = 0;
	 spi_write(2048);
	 TA2CCTL0 &= ~COV;
}
