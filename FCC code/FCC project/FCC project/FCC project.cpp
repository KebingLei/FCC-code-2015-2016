/*
 * FCC_project.cpp
 *
 * Created: 2015-09-19 2:00:27 PM-9
 *  Author: Me
 */ 


#define F_CPU 80000000

#include <avr32/io.h>
#include <avr32/rtc_100.h>
#include "Output_defs.h"


avr32_rtc_isr_t rtcint;
avr32_rtc_imr_t rtcimr;

int main(void)
{
	rtcint.topi=0;
	rtcimr.topi=0;
	OP_GPERS |= OP1 | OP2 | OP3 | OP4 | OP5 | OP6 | OP7 | OP8 | OP9 | OP10;
	OP_ODERS |= OP1 | OP2 | OP3 | OP4 | OP5 | OP6 | OP7 | OP8 | OP9 | OP10;
	LED_GPERS |= LED0 | LED1 | LED2 | LED3;
	LED_ODERS |= LED0 | LED1 | LED2 | LED3;
    while(1)
    {
		OP_OVRS = OP1;
		LED_OVRS = LED1;
    }
}