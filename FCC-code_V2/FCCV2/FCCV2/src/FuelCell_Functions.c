/*
 * FuelCell_Functions.c
 *
 * Created: 2015-11-09 11:07:53 AM
 *  Author: Reegan
 */ 

#include "asf.h"
#include "FuelCell_Functions.h"
#include "FC_error_codes.h"
#include "digital_IO_defs.h"
#include "FuelCell_ADC.h"
#include "FuelCell_PWM.h"
#include <math.h>

int adcvals_0[7];
int adcvals_1[8];

//create value for timer
unsigned long counta;
void millis_init(void)
{
	//AST generic clock 8MHz / (2*(249+1) = 16kHz
	scif_gc_setup(AVR32_SCIF_GCLK_AST,SCIF_GCCTRL_RC8M,AVR32_SCIF_GC_DIV_CLOCK,249);
	// Now enable the generic clock
	scif_gc_enable(AVR32_SCIF_GCLK_AST);
	//set up timer
	ast_init_counter(&AVR32_AST,AST_OSC_GCLK,4,counta);  //16kHz / (2^(4+1)) = 1kHz
	ast_enable(&AVR32_AST);
}

unsigned long millis(void)
{
	return(ast_get_counter_value(&AVR32_AST)); 
}

unsigned int TEMP_OPT;
unsigned int AVE_TEMP; 
unsigned int temp_error;
//float accumulated_temp_error = 0;
//unsigned int PIDP; //speed proportional to temp error
//unsigned int PIDI; //integral -> accumulated error
//unsigned int PIDD; //Derivative -> change in temp
//I'm only going to make speed depend on temp error
unsigned int pid_temp_control(void)
{
	//Topt = 0.53I + 26.01 in C and Amps
	//= (53 * I) / 100 + 299160 in mK and mA
	TEMP_OPT = (53*get_CAPVOLT()) / 100 + 299160;
	AVE_TEMP = (get_FCTEMP1() + get_FCTEMP2())/2;
	
	temp_error = (TEMP_OPT - AVE_TEMP) / 1000; //in Kalvin
	//accumulated_temp_error = accumulated_temp_error + temp_error;
	
	//fan speed = 0 to 20 //should really increase this value
	//max is 20. min is 5. diff is 15
	//run fan at 1% above min per degree K it is above T_OPT
	FANUpdate(5+(unsigned int)temp_error*20/100);
	return(1);
}

unsigned int FC_check_alarms(unsigned int fc_state)
{
	//to do: make alarms smarter based on fc_state.
	unsigned int error_msg = 0;
	switch 
	if(gpio_get_pin_value(CAPCON) == 0)
	{
		error_msg |= FC_ERR_CAP_DISC;
	}
	if(gpio_get_pin_value(FCCON) == 0)
	{
		error_msg |= FC_ERR_FC_DISC;
	}
	if(gpio_get_pin_value(SYSOK) == 0) 
	{
		error_msg |= FC_ERR_H2OK_LOW;
	}
	if((get_FCTEMP1() <= LOW_TEMP_THRES)|(get_FCTEMP2() <= LOW_TEMP_THRES))
	{
		error_msg |= FC_ERR_TEMP_L;
	}
	if((get_FCTEMP1() >= HIGH_TEMP_THRES)|(get_FCTEMP2() >= HIGH_TEMP_THRES))
	{
		error_msg |= FC_ERR_TEMP_H;
	}
	//only check pressure in purge, charge and run
	if((get_FCPRES() >= HIGH_PRES_THRES))
	{
		error_msg |= FC_ERR_PRES_H;
	}
	if((get_FCPRES() <= LOW_PRES_THRES))
	{
		error_msg |= FC_ERR_PRES_L;
	}
	if(get_FCCURR() >= OVER_CUR_THRES)
	{
		error_msg |= FC_ERR_OVER_CUR;
	}
	if(get_FCCURR() <= UNDER_CUR_THRES)
	{
		error_msg |= FC_ERR_UND_CUR;	}	if(get_FCVOLT() >= OVER_VOLT_THRES)
	{
		error_msg |= FC_ERR_OVER_VOLT;
	}
	
	return(error_msg);
}

unsigned int FC_standby(void)
{
	unsigned int fc_state;
	if (gpio_get_pin_value(START))
	{
		fc_state = FC_STATE_STARTUP_FANS;
		gpio_clr_gpio_pin(LED_STOP);
		gpio_set_gpio_pin(LED_START);
	}
	else
	{
		//make sure fuel cell stays off
		//Supply valve closed
		gpio_clr_gpio_pin(H2_VALVE);
		//purge valve closed
		gpio_clr_gpio_pin(PURGE_VALVE);
		//relays open
		gpio_clr_gpio_pin(MOTOR_RELAY);
		gpio_clr_gpio_pin(RES_RELAY);
		gpio_clr_gpio_pin(CAP_RELAY);
		//led's off
		gpio_clr_gpio_pin(LED_RUN);
		gpio_clr_gpio_pin(LED_START);
		gpio_clr_gpio_pin(LED_STOP);
		//fan off?
		//or low
		FANUpdate(4);
		fc_state = FC_STATE_STANDBY;
	}
	return(fc_state);
}

unsigned int tachometer_test = 0;
unsigned int FC_startup_fans(void)
{
	unsigned int fc_state;
	//relays open
	gpio_clr_gpio_pin(START_RELAY);
	gpio_clr_gpio_pin(MOTOR_RELAY);
	//valves closed
	gpio_clr_gpio_pin(H2_VALVE);
	gpio_clr_gpio_pin(PURGE_VALVE);
	
	//set fans to min
	FANUpdate(4);
	//read fan tachometer to ensure fans are spinning
	//increment if tach is 0 and reads 1
	if(tachometer_test == 0)
	{
		if(gpio_get_pin_value(FAN_TACH)==1)
		{
			tachometer_test = 1;
		}
	}
	fc_state = FC_STATE_STARTUP_FANS; //keep looping in this function
	
	//then wait for it to go low again (then the fan is spinning)
	if(tachometer_test == 1)
	{
		if(gpio_get_pin_value(FAN_TACH) == 0)
		{
			//fan is spinning go to startup
			fc_state = FC_STATE_STARTUP_H2;
		}
	}
	return(fc_state);
}


unsigned int FC_startup_h2(void)
{
	unsigned int fc_state;
	
	//open h2 valve
	gpio_set_gpio_pin(H2_VALVE);
	//purge valve closed
	gpio_clr_gpio_pin(PURGE_VALVE);
	//relays open
	gpio_clr_gpio_pin(START_RELAY);
	gpio_clr_gpio_pin(MOTOR_RELAY);
	
	//input h2 until voltage reaches 30
	if (get_FCVOLT() < (30000)) //if voltage is less than 30V
	{
		//keep the hydrogen coming
		fc_state = FC_STATE_STARTUP_H2;
	}
	else
	{
		//voltage is 30 then do start up purge
		fc_state = FC_STATE_STARTUP_PURGE;
	}
	return(fc_state);
}


unsigned int purge_timer2 = 0; //used for timing how long purge valve is open
unsigned int FC_startup_purge(void)
{
	//Purge step isn't skipped on !Rescon to get air out of lines on startup. what???
	unsigned int fc_state;
	
	//h2 valve still open
	gpio_set_gpio_pin(H2_VALVE);
	//close startup relay
	gpio_set_gpio_pin(START_RELAY);
	//motor relay still open
	gpio_clr_gpio_pin(MOTOR_RELAY);
	
	//open purge valve and start timer
	if(gpio_get_gpio_pin_output_value(PURGE_VALVE) == 0)
	{
		purge_timer2 = millis();
	}
	gpio_set_gpio_pin(PURGE_VALVE);
	gpio_set_gpio_pin(LED0);
	
	//balazs has pseudo code purge for 3 seconds
	if(millis() - purge_timer2 < 3000)
	{
		fc_state = FC_STATE_STARTUP_PURGE;
	}
	else //3 seconds are over
	{
		//close purge valve
		gpio_clr_gpio_pin(PURGE_VALVE);
		gpio_clr_gpio_pin(LED0);
		
		//open startup relay
		gpio_clr_gpio_pin(START_RELAY);
		//other relays still open
		fc_state = FC_STATE_STARTUP_CHARGE;
	}
	return(fc_state);
}


unsigned int purge_timer1; //using for keeping track of time between purges 
unsigned int delta_purge_time;
U64 mAms_since_last_purge;
unsigned int purge_state = FIRST_PURGE_CYCLE;
unsigned int FC_startup_charge(void)
{
	unsigned int fc_state = FC_STATE_STARTUP_CHARGE; //will keep charging untill state exits
	//Skip charge step if resistors aren't connected
	//pseudo code checked RESCON here
	//however, RESCON == 0 will trigger and alarm anyway so I won't check

	if(purge_state == FIRST_PURGE_CYCLE)
	{
		purge_timer1 = millis();
		purge_state = PURGE_VALVE_CLOSED; //fuel cell running purge valve closed //prevents timer from being reset
	}
	//keep track of columbs of charge produced since last purge
	delta_purge_time = millis() - purge_timer1;
	if(delta_purge_time > PURGE_INTEGRATION_INTERVAL)
	{
		mAms_since_last_purge += delta_purge_time * get_FCCURR();
		purge_timer1 = millis();
	}
	//we shouldn't need to purge in this state because 2300C/40V = 57.5F of capacitance we can fill
	//but it still impacts time till next purge
	
	//charging capacitors through resistor to avoid temporary short circuit
	if (get_CAPVOLT() < 40000) //if voltage is less than 40V
	{
		//close resistor relay
		gpio_set_gpio_pin(RES_RELAY);
		gpio_set_gpio_pin(LED1); //make led1 indicate charging state?
		//other relays open still
		//H2_valve open
		//purge valve still closed
		//fc_State still charge
	}
	else //caps are charged
	{
		//turn off led1
		gpio_clr_gpio_pin(LED1);
		
		//open resistor relay
		
		//delay how long?
		//why not one adc cycle?
		if(gpio_get_gpio_pin_output_value(RES_RELAY) == 1)
		{
			gpio_clr_gpio_pin(RES_RELAY);	
			return(fc_state); //state will run again
		}
		
		//close cap relay
		//another delay
		if(gpio_get_gpio_pin_output_value(CAP_RELAY == 0))
		{
			gpio_set_gpio_pin(CAP_RELAY);
			return(fc_state);
		}
		
		//close motor relay
		gpio_set_gpio_pin(MOTOR_RELAY);
		
		//go to main run state
		fc_state = FC_STATE_RUN;
		gpio_clr_gpio_pin(LED_START);
		gpio_set_gpio_pin(LED_RUN);
	}
	return(fc_state);
}


unsigned int fan_update_timer;
unsigned int FC_run(void)
{
	unsigned int fc_state;
	
	if(millis() - fan_update_timer > FANUPDATE_INTERVAL)
	{
		//pid fan control for temp regulation
		pid_temp_control();
		fan_update_timer = millis();
	}
	
	//purge control: //purge based on amount of charge extracted from hydrogen (1 C = amp * sec)
	
	delta_purge_time = millis() - purge_timer1;
	if(delta_purge_time > PURGE_INTEGRATION_INTERVAL)
	{
		mAms_since_last_purge += delta_purge_time * get_FCCURR();
		purge_timer1 = millis();
	}
	
	if (mAms_since_last_purge > PURGE_THRESHOLD) //time to purge
	{
		//open purge valve
		gpio_set_gpio_pin(PURGE_VALVE);
		
		//we restart counting mAms as soon as valve opens
		//reset mAms sum
		mAms_since_last_purge = 0;
		//reset timer1
		purge_timer1 = millis();
		
		//start purge timer 2 to time purge
		purge_timer2 = millis();
		purge_state = PURGE_VALVE_OPEN; //purge valve open
		//set led0 on because why not
		gpio_set_gpio_pin(LED0);
	}
	
	if(purge_state == PURGE_VALVE_OPEN) //if purge valve is open
	{
		if(millis() - purge_timer2 > PURGE_TIME) //if it has completed purge //is there some way to use flow meter instead of a fixed time?
		{
			//close purge valve
			gpio_clr_gpio_pin(PURGE_VALVE);
			purge_state = PURGE_VALVE_CLOSED; //purge valve closed
			
			//turn led0 off
			gpio_clr_gpio_pin(LED0);
		}
	}
	
	fc_state = FC_STATE_RUN;
	return(fc_state);
}

unsigned int FC_shutdown(void)
{
	unsigned int fc_state;
	gpio_clr_gpio_pin(LED_RUN);
	gpio_set_gpio_pin(LED_STOP);
	//close valves
	gpio_clr_gpio_pin(H2_VALVE);
	gpio_clr_gpio_pin(PURGE_VALVE);
	//close relays
	gpio_clr_gpio_pin(MOTOR_RELAY);
	gpio_clr_gpio_pin(START_RELAY);
	
	if(0)
	{
		//option of leaving shutdown state and restarting
		fc_state = FC_STATE_STANDBY;
	}
	else
	{
		fc_state = FC_STATE_SHUTDOWN;
	}
	return(fc_state);
}

unsigned int FC_alarm(void)
{
	unsigned int fc_state;
	gpio_clr_gpio_pin(LED_RUN);
	gpio_set_gpio_pin(LED_ERROR);
	gpio_set_gpio_pin(LED_STOP);
	//close valves
	gpio_clr_gpio_pin(H2_VALVE);
	gpio_clr_gpio_pin(PURGE_VALVE);
	//close relays
	gpio_clr_gpio_pin(MOTOR_RELAY);
	gpio_clr_gpio_pin(START_RELAY);
	
	fc_state = FC_STATE_ALARM;
	//manual reset required to exit alarm state
	return(fc_state);
}
