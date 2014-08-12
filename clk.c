/*
 * clk.c
 *
 *  Created on: 2014年8月10日
 *      Author: beny
 */
#include "msp430.h"

void SetVcoreUp (unsigned int level);

void clk_init()
{
	P1DIR |= BIT0;                            // ACLK set out to pins
	  P1SEL |= BIT0;
	  P2DIR |= BIT2;                            // SMCLK set out to pins
	  P2SEL |= BIT2;
	  P7DIR |= BIT7;                            // MCLK set out to pins
	  P7SEL |= BIT7;


	  P5SEL |= BIT2+BIT3;                       // Port select XT2
	  SetVcoreUp (0x01);
	  SetVcoreUp (0x02);
	  SetVcoreUp (0x03);

	  UCSCTL6 &= ~XT2OFF;                       // Enable XT2
	  UCSCTL3 |= SELREF_2;                      // FLLref = REFO
	                                            // Since LFXT1 is not used,
	                                            // sourcing FLL with LFXT1 can cause
	                                            // XT1OFFG flag to set
	  UCSCTL4 |= SELA_2;                        // ACLK=REFO,SMCLK=DCO,MCLK=DCO
	  __bis_SR_register(SCG0);                  // Disable the FLL control loop
	  UCSCTL0 = 0x0000;                         // Set lowest possible DCOx, MODx
	  UCSCTL1 = DCORSEL_7;                      // Select DCO range 50MHz operation
	  UCSCTL2 = FLLD_0 + 762;                   // Set DCO Multiplier for 25MHz
	                                              // (N + 1) * FLLRef = Fdco
	                                              // (762 + 1) * 32768 = 25MHz
	                                              // Set FLL Div = fDCOCLK/2
	  __bic_SR_register(SCG0);                  // Enable the FLL control loop

	  __delay_cycles(782000);


	  // Loop until XT1,XT2 & DCO stabilizes - in this case loop until XT2 settles
	  do
	  {
	    UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
	                                            // Clear XT2,XT1,DCO fault flags
	    SFRIFG1 &= ~OFIFG;                      // Clear fault flags
	  }while (SFRIFG1&OFIFG);                   // Test oscillator fault flag

	  //UCSCTL6 &= ~XT2DRIVE0;                    // Decrease XT2 Drive according to
	                                            // expected frequency
	  UCSCTL4 |= SELS__XT2CLK ;// +SELM__DCOCLK;               // SMCLK=MCLK=XT2
}

void SetVcoreUp (unsigned int level)
{
  // Open PMM registers for write
  PMMCTL0_H = PMMPW_H;
  // Set SVS/SVM high side new level
  SVSMHCTL = SVSHE + SVSHRVL0 * level + SVMHE + SVSMHRRL0 * level;
  // Set SVM low side to new level
  SVSMLCTL = SVSLE + SVMLE + SVSMLRRL0 * level;
  // Wait till SVM is settled
  while ((PMMIFG & SVSMLDLYIFG) == 0);
  // Clear already set flags
  PMMIFG &= ~(SVMLVLRIFG + SVMLIFG);
  // Set VCore to new level
  PMMCTL0_L = PMMCOREV0 * level;
  // Wait till new level reached
  if ((PMMIFG & SVMLIFG))
    while ((PMMIFG & SVMLVLRIFG) == 0);
  // Set SVS/SVM low side to new level
  SVSMLCTL = SVSLE + SVSLRVL0 * level + SVMLE + SVSMLRRL0 * level;
  // Lock PMM registers for write access
  PMMCTL0_H = 0x00;
}

