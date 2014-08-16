#include <msp430.h> 

/*
 * main.c
 */
#include "freq_cap.h"
#include "clk.h"
#include "spi_dac8043.h"
#include "lcd12864.h"
#include "MatrixKey.h"
#include "adc12.h"
extern float freq;
unsigned int thd=0,hud=0,dec=0,sig=0;

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    clk_init();
    lcd_init();
	write_zi(2,0,"DAC Scale:");
	write_zi(0,0,"freq:");
    spi_init();
    P7DIR |= BIT4;
    P8DIR |= BIT1+BIT2;
    adc12_init();
    key_Init();
	freq_cap_init();
	_EINT();

	while(1);
	return 0;
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER1_A0_VECTOR))) TIMER1_A0_ISR (void)
#else
#error Compiler not supported!
#endif
{
	char key = KeyScan();
	switch(key) {
	case 0:
	case 1:
	case 2:
		thd = hud;
		hud = dec;
		dec	= sig;
		sig = key +1;
		break;
	case 3://Add
		sig++;
		if(sig == 10){
			sig = 0;
			dec++;
			if(dec == 10) {
				dec = 0;
				hud++;
				if(hud == 1) {
					hud = 0;
					sig = 0;
					dec = 0;
				}
			}
		}
		break;
	case 4:
	case 5:
	case 6:
		thd = hud;
		hud = dec;
		dec	= sig;
		sig = key;
		break;
	case 7://Dece
		sig --;
		if (sig == -1){
			sig = 9;
			dec --;
			if (dec == -1){
				dec = 9;
				hud--;
				if (hud==-1){
					hud = 0;
				}
			}
		}
		break;
	case 8:
	case 9:
	case 10:
		thd = hud;
		hud = dec;
		dec	= sig;
		sig = key - 1;
	case 11:break;
	case 12:break;
	case 13:
		thd = hud;
		hud = dec;
		dec	= sig;
		sig = 0;
	case 14:
		break;
	case 15:
		MPY = thd;
		OP2 = 1000;
		MAC = hud;
		OP2 = 100;
		MAC = dec;
		OP2 = 10;
		spi1_write(RESLO+sig);
	default:break;
	}
	unsigned char te[5]={thd+'0',hud+'0',dec+'0',sig+'0','\0'};
	write_zi(3,0,te);
	freq = 4783+ADC12MEM0 << 12;
	unsigned char fr[5]={'\0'};
	fr[0] = (int)freq/1000+'0';
	fr[1] = ((int)freq%1000)/100+'0';
	fr[2] = ((int)freq%100)/10+'0';
	fr[3] = ((int)freq%10)+'0';
	write_zi(1,0,fr);
}
