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
int thd=0,hud=0,dec=0,sig=0;
char Cap_Flg=0;
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
   // __delay_cycles(720000);
    clk_init();
    lcd_init();
	write_zi(1,0,"DAC Scale:");
	write_zi(0,0,"Freq: ");
	write_zi(2,0,"Power:");
    spi_init();
    P7DIR |= BIT4;
    P8DIR |= BIT1+BIT2;
    adc12_init();
    key_Init();
	freq_cap_init();
	spi1_write(0);
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
	if (key != 20){
		write_zi(3,0,"                             ");
	}
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
				if(hud == 10) {
					thd ++;
					if(thd == 5) {
						thd = 0;
					}
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
					hud = 9;
					thd --;
					if(thd == -1) {
						thd = 4;
					}
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
		break;
	case 11:
		if(Cap_Flg == 0) {
			Cap_Flg =1;
			write_zi(3,0,"Start Mean Freq");
		}
		else {
			Cap_Flg = 0;
			write_zi(3,0,"Stop Mean Freq");
		}
		break;
	case 12:break;
	case 13:
		thd = hud;
		hud = dec;
		dec	= sig;
		sig = 0;
		break;
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
		write_zi(3,0,"Applied!");
		break;
	default:break;
	}
	unsigned char te[5]={thd+'0',hud+'0',dec+'0',sig+'0','\0'};
	write_zi(1,5,te);
}
