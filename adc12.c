/*
 * adc12.c
 *
 *  Created on: 2014年8月15日
 *      Author: beny
 */
#include "msp430.h"
#include "adc12.h"
#include "notch_filter.h"
#include"spi_dac8043.h"
void tb_init()
{
   //P4SEL |= BIT1;                            // P4 option select
   //P4DIR |= BIT1;
   TB0CCR0 =  100-1;				//40kHz
   TB0CCTL1 = OUTMOD_7;
   TB0CCR1 = 40;
   TB0CTL = TBSSEL_2 + MC_1 +TBCLR; //set SMCLK source, set Up-Mode
}

void adc12_init()
{
	volatile unsigned int i;
	P6SEL |= BIT1;			//Pin select
	REFCTL0 &= ~REFMSTR;
	ADC12CTL0 = ADC12ON + ADC12SHT0_3 + ADC12REFON + ADC12REF2_5V;     //Turn on ADC12 ,set sampling time 8 ADC12CLK cycles
	                                             //set multiple sample and conversion
	ADC12CTL1 = ADC12SHP+ADC12CONSEQ_2 + ADC12SHS_3;       //Using sampling timer ,set repeat mode,
	                                                          //sample-and-hold source select CCR1 TB0
	ADC12CTL2 |= ADC12DF; //Signed Format
	ADC12MCTL0 =  ADC12INCH_1 + ADC12SREF_1;  //A1,Vref+ = AVcc,Vref- = AVss
	for ( i=0; i<0x30; i++);                  // Delay for reference start-up
	ADC12IE = BIT0; //Enable ADC12IFG0
	ADC12CTL0 |= ADC12ENC; //Enable conversion
	tb_init();
	ADC12CTL0 |= ADC12SC;   //Start conversion
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=ADC12_VECTOR
__interrupt void ADC12ISR (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC12_VECTOR))) ADC12ISR (void)
#else
#error Compiler not supported!
#endif
{

  switch(__even_in_range(ADC12IV,34))
  {
  case  0: break;                           // Vector  0:  No interrupt
  case  2: break;                           // Vector  2:  ADC overflow
  case  4: break;                           // Vector  4:  ADC timing overflow
  case  6:                                  // Vector  6:  ADC12IFG0
	  ADC12IE &= ~BIT0;
	  P8OUT |= BIT1;
	  volatile fixed ta=filter(ADC12MEM0);
	  if (ta < -32768)
		  ta = -32768;
	  if(ta > 32767)
		  ta = 32767;
	 spi_write(1760);
	 //spi_write(ADC12MEM0 >> 4);
	  ADC12IE |= BIT0;
	  P8OUT &= ~BIT1;
    break;
  default: break;
  }
}

