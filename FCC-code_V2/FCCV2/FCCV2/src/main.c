/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a> //this site is useless
 */

/*
to do list:
get good adc conversions (test readings)
can bus
fan pid control code
test test test
*/

#include <stdlib.h>
#include <stdio.h>
#include "asf.h"
#include "FuelCell_ADC.h"
#include "FuelCell_Functions.h"
#include "FuelCell_PWM.h"
#include "digital_IO_defs.h"
#include "FC_error_codes.h"
#include "FuelCell_USART.h"
#include "FuelCell_CAN.h"
#include "pid.h"

char str [20]; //buffer for storing strings

unsigned int error_msg;
unsigned int fc_state = FC_STATE_STANDBY;

unsigned int data_log_stagger = 0;
unsigned int data_log_timer;
#define DATA_LOG_INTERVAL 250

int main (void)
{
	board_init();
	
	//Start of main loop
	while(1)
	{
		//read analog inputs
		StartADC_Sequencers();
		ReadADC_Sequencers();

		error_msg = FC_check_alarms(fc_state);
		if(error_msg)
		{
			fc_state = FC_STATE_ALARM;
		}
		
		
		//main state machine
		switch (fc_state)
		{
		case FC_STATE_STANDBY:
			fc_state = FC_standby();
			
		case FC_STATE_SHUTDOWN:
			fc_state = FC_shutdown();
		
		case FC_STATE_STARTUP_FANS:
			fc_state = FC_startup_fans();
		
		case FC_STATE_STARTUP_H2:
			fc_state = FC_startup_h2();
			
		case FC_STATE_STARTUP_PURGE:
			fc_state = FC_startup_purge();
		
		case FC_STATE_STARTUP_CHARGE:
			fc_state = FC_startup_charge();	
		
		case FC_STATE_RUN:
			fc_state = FC_run();
			
		case FC_STATE_ALARM:
			fc_state = FC_alarm();			
		}
		
		//all data logging occurs after state machine
		//that way if alarm is triggered it is delt with before
			
		//send data over serial
		//stagger messages to be able to continue checking levels
		//with out a huge serial delay (every few messages is a couple milliseconds)
		switch (data_log_stagger)
		{
			case 0:
			if(millis()- data_log_timer > DATA_LOG_INTERVAL)
			{
				data_log_stagger = 1;
				data_log_timer = millis();
				if(error_msg)
				{
					sprintf(str,"ERROR %d\n",error_msg);
					usart_write_line(EXAMPLE_USART,str);
				}
				sprintf(str,"LASTPURGE %d",get_time_between_last_purges());
				usart_write_line(EXAMPLE_USART,str);
			}
			case 1:
			sprintf(str,"FCVOLT %d\n",get_FCVOLT());
			usart_write_line(EXAMPLE_USART,str);
			sprintf(str,"FCCURR %d\n",get_FCCURR());
			usart_write_line(EXAMPLE_USART,str);
			sprintf(str,"FCPRES %d\n",get_FCPRES());
			usart_write_line(EXAMPLE_USART,str);
			data_log_stagger = 2;
			case 2:
			sprintf(str,"FCTEMP1 %d\n",get_FCTEMP1());
			usart_write_line(EXAMPLE_USART,str);
			sprintf(str,"FCTEMP2 %d\n",get_FCTEMP2());
			usart_write_line(EXAMPLE_USART,str);
			data_log_stagger = 3;
			case 3:
			sprintf(str,"AMBTEMP0 %d\n",get_AMBTEMP0());
			usart_write_line(EXAMPLE_USART,str);
			sprintf(str,"AMBTEMP1 %d\n",get_AMBTEMP1());
			usart_write_line(EXAMPLE_USART,str);
			sprintf(str,"AMBTEMP2 %d\n",get_AMBTEMP2());
			usart_write_line(EXAMPLE_USART,str);
			sprintf(str,"AMBTEMP3 %d\n",get_AMBTEMP3());
			usart_write_line(EXAMPLE_USART,str);
			data_log_stagger = 4;
			case 4:
			sprintf(str,"CAPVOLT %d\n",get_CAPVOLT());
			usart_write_line(EXAMPLE_USART,str);
			data_log_stagger = 0;
		}
	}
}