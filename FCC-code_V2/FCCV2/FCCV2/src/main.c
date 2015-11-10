/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application templat
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal  function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include "asf.h"
#include "FuelCell_ADC.h"
#include "FuelCell_Functions.h"
#include "FuelCell_PWM.h"
#include "digital_IO_defs.h"

unsigned long time1 = 0;
int val;

int main (void)
{
	board_init();
	
	StartADC_Sequencers();
	ReadADC_Sequencers();
	
	val = FCVOLTValue;
	
	if(time1 - millis() > 1000)
	{
		gpio_tgl_gpio_pin(LED0);
		time1 = millis();
		FANUpdate(500); //Note it would be really cool if this function set the actual rpm but it doesn't
	}
	
}