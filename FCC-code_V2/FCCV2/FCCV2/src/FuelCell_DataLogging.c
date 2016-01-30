/*
 * FuelCell_DataLogging.c
 *
 * Created: 2016-01-16 9:07:07 AM
 *  Author: Reegan
 */ 
#include <stdlib.h>
#include <stdio.h>
#include "asf.h"
#include "FuelCell_USART.h"
#include "FuelCell_ADC.h"
#include "FuelCell_Functions.h"
#include "FuelCell_DataLogging.h"
#include "millis_function.h"
#include "FuelCell_PWM.h"
#include "FuelCell_mode_Select.h"
#include "digital_IO_defs.h"

char str [20]; //buffer for storing strings. set to length of longest string needed to avoid wasting ram

unsigned int usart_data_display_stagger = 0;
unsigned int usart_data_display_timer;

void usart_data_display(unsigned int fc_state, unsigned int error_msg)
{
	//all data logging occurs after state machine executes
	//that way if alarm is triggered it is dealt with before
	//sending data over serial
	//stagger messages to be able to continue checking levels
	//without a huge serial delay (every message is about a millisecond)
	
	switch (usart_data_display_stagger)
	{
		case 0:
		if(millis()- usart_data_display_timer > USART_DATA_DISPLAY_INTERVAL)
		{
			usart_data_display_stagger = 1;
			usart_data_display_timer = millis();
			sprintf(str,"\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r");
			usart_write_line(DISPLAY_USART,str);
			
			#ifdef TEST_BENCH_MODE
			usart_write_line(DISPLAY_USART,"You are in test bench mode\n\r");
			usart_write_line(DISPLAY_USART,"Do NOT connect the fuel cell!\n\r");
			#endif
			
			if(error_msg)
			{
				sprintf(str,"ERROR %d\n\r",error_msg);
				usart_write_line(DISPLAY_USART,str);
			}
			sprintf(str,"NUMBER OF PURGES %d\n\r",get_number_of_purges());
			usart_write_line(DISPLAY_USART,str);
			sprintf(str,"LASTPURGE %d\n\r",get_time_between_last_purges());
			usart_write_line(DISPLAY_USART,str);
			sprintf(str,"STATE %d\n\r",fc_state);
			usart_write_line(DISPLAY_USART,str);
			sprintf(str,"\n\r");
			usart_write_line(DISPLAY_USART,str);
		}
		break;
		
		case 1:
		sprintf(str,"FCVOLT %d\n\r",get_FCVOLT());
		usart_write_line(DISPLAY_USART,str);
		sprintf(str,"FCCURR %d\n\r",get_FCCURR());
		usart_write_line(DISPLAY_USART,str);
		sprintf(str,"FCPRES %d\n\r",get_FCPRES());
		usart_write_line(DISPLAY_USART,str);
		sprintf(str,"\n\r");
		usart_write_line(DISPLAY_USART,str);
		usart_data_display_stagger = 2;
		break;
		
		case 2:
		sprintf(str,"FCTEMP1 %d\n\r",get_FCTEMP1());
		usart_write_line(DISPLAY_USART,str);
		sprintf(str,"FCTEMP2 %d\n\r",get_FCTEMP2());
		usart_write_line(DISPLAY_USART,str);
		sprintf(str,"FANSpeed %d\n\r",get_FANSpeed());
		usart_write_line(DISPLAY_USART,str);
		sprintf(str,"OPT_TEMP %d\n\r",calc_opt_temp());
		usart_write_line(DISPLAY_USART,str);
		sprintf(str,"\n\r");
		usart_write_line(DISPLAY_USART,str);
		usart_data_display_stagger = 3;
		break;
		
		case 3:
		/* AMBTEMP not connected
		sprintf(str,"AMBTEMP0 %d\n\r",get_AMBTEMP0());
		usart_write_line(DISPLAY_USART,str);
		sprintf(str,"AMBTEMP1 %d\n\r",get_AMBTEMP1());
		usart_write_line(DISPLAY_USART,str);
		sprintf(str,"AMBTEMP2 %d\n\r",get_AMBTEMP2());
		usart_write_line(DISPLAY_USART,str);
		sprintf(str,"AMBTEMP3 %d\n\r",get_AMBTEMP3());
		usart_write_line(DISPLAY_USART,str);
		sprintf(str,"\n\r\n\r\n\r");
		usart_write_line(DISPLAY_USART,str);
		*/
		usart_data_display_stagger = 4;
		break;
		
		case 4:
		sprintf(str,"CAPVOLT %d\n\r",get_CAPVOLT());
		usart_write_line(DISPLAY_USART,str);
		sprintf(str,"\n\r");
		usart_write_line(DISPLAY_USART,str);
		usart_data_display_stagger = 0;
		break;
	}
}

unsigned int usart_data_log_timer = 0;

void usart_data_logging(unsigned int fc_state, unsigned int error_msg)
{
	if(millis()-usart_data_log_timer > USART_DATA_LOG_INTERVAL)
	{
	//start with message time stamp
	sprintf(str,"MILLIS %lu\n\r",millis());
	usart_write_line(LOG_USART,str);

	//FC DATA
	sprintf(str,"FCVOLT %d\n\r",get_FCVOLT());
	usart_write_line(LOG_USART,str);
	sprintf(str,"FCCURR %d\n\r",get_FCCURR());
	usart_write_line(LOG_USART,str);
	sprintf(str,"FCTEMP1 %d\n\r",get_FCTEMP1());
	usart_write_line(LOG_USART,str);
	sprintf(str,"FCTEMP2 %d\n\r",get_FCTEMP2());
	usart_write_line(LOG_USART,str);
	sprintf(str,"FCPRES %d\n\r",get_FCPRES());
	usart_write_line(LOG_USART,str);
	
	sprintf(str,"CAPVOLT %d\n\r",get_CAPVOLT());
	usart_write_line(LOG_USART,str);
	
	sprintf(str,"OPTTEMP %d\n\r",calc_opt_temp());
	usart_write_line(LOG_USART,str);

	//purge data
	sprintf(str,"LAST_PURGE %d\n\r",get_time_between_last_purges());
	usart_write_line(LOG_USART,str);
	sprintf(str,"NUM_PURGE %d\n\r",get_number_of_purges());
	usart_write_line(LOG_USART,str);
	sprintf(str,"CHARGE_EXTRACT %d\n\r",get_total_charge_extracted());
	usart_write_line(LOG_USART,str);
	
	//state information
	sprintf(str,"STATE %d\n\r",fc_state);
	usart_write_line(LOG_USART,str);
	sprintf(str,"ERROR %d\n\r",error_msg);
	usart_write_line(LOG_USART,str);
	sprintf(str,"PURGE_VALVE %d\n\r",gpio_get_gpio_pin_output_value(PURGE_VALVE));
	usart_write_line(LOG_USART,str);
	usart_data_log_timer = millis();
	}
}




