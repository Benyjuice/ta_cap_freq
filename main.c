#include <msp430.h> 

/*
 * main.c
 */
#include "freq_cap.h"
#include "clk.h"
#include "spi_dac8043.h"
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    clk_init();
    spi_init();
	freq_cap_init();

	while(1);
	return 0;
}
