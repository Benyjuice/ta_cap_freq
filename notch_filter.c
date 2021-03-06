/*
 * notch_filter.c
 *
 *  Created on: 2014年8月15日
 *      Author: beny
 */
#include "msp430.h"
#include "notch_filter.h"
#define u 0x200 //step=0.4654
#define point 40

const fixed wsin[point]={
		0x0000,0x2d41,0x4000,0x2d41,
		0x0000,0xd2bf,0xc000,0xd2bf};
//const float wsin[point] = {0.0 , 0.8660 , -0.8660};
//const float wcos[point] = {1.0, -0.5 , 0.5};

fixed filter(fixed t)
{
  static fixed ws = 0;
  static fixed wc = 0;
  static fixed result;
  static unsigned char i_s=0;
  static unsigned char i_c=0;
  fixed re,err;
  // calculate result = (int)(ws*wsin[i]+wc*wcos[i]);
  MPY32CTL0 |= MPYFRAC;                      // Set fractional mode
  MPYS = ws;
  OP2 = wsin[i_s];
  MACS = wc;
  OP2 = wsin[i_c];
  result = RESHI;
  // end of calculate


  // calculate err = u*(ADC12MEM0 - result);
  re = t - result; //ADC12MEM0 need to be transfered to Q15 format
  MPYS = re;
  OP2 = u;
  err = RESHI;
  //end of calculate

  //calculate ws = ws + err*wsin[i];
  MPYS = err;
  OP2 = wsin[i_s];
  ws = ws + RESHI;
  //end

  //calculate wc = wc + err*wcos[i];
  MPYS = err;
  OP2 = wsin[i_c];
  wc = wc + RESHI;
  //end

  i_s ++;
  if(i_s == point)
	  i_s = 0;
  i_c = i_s + 10;
  if (i_c >= point)
	  i_c -= 40;
  MPY32CTL0 &= ~MPYFRAC;
  return re;
}


/*int filter(int t)
{
  static int ws = 0;
  static int wc = 0;
  static float result, am;
  static unsigned char i = 0;
  float err;

  result = ws*wsin[i] + wc*wcos[i];
  err = u*(t - result);
  ws += err*wsin[i];
  wc += err*wcos[i];
  am = ws*ws + wc*wc;
  i++;
  if (point == i)
    i = 0;
  return am;
}*/


