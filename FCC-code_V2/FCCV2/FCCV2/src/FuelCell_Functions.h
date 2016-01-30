/*
 * FuelCell_Functions.h
 *
 * Created: 2015-11-09 11:08:25 AM
 *  Author: Reegan
 *
*/
#ifndef FUELCELL_FUNCTIONS_H_


//different fuel cell states
#define FC_STATE_STANDBY 0
#define FC_STATE_SHUTDOWN 1
#define FC_STATE_STARTUP_FANS 2
#define FC_STATE_STARTUP_H2 3
#define FC_STATE_STARTUP_PURGE 4
#define FC_STATE_STARTUP_CHARGE 5
#define FC_STATE_RUN 6
#define FC_STATE_ALARM 8

//different substates while running
#define FIRST_PURGE_CYCLE 0
#define PURGE_VALVE_OPEN 2
#define PURGE_VALVE_CLOSED 1

#define FANUPDATE_INTERVAL 100

#define PURGE_INTEGRATION_INTERVAL 10 //10 ms
#define PURGE_THRESHOLD 2300000000 //2300 C from fuel cell documentation. units mA * ms
//this is larger than max 32 bit value (nice)
#define PURGE_TIME 200 //get this value from datasheet
#define MAX_PURGE_INTERVAL 0 //max time between purges regardless of current output

unsigned int get_time_between_last_purges(void);
unsigned int get_number_of_purges(void);
unsigned int get_total_charge_extracted(void);
int calc_opt_temp(void);

//state functions
unsigned int FC_standby(void);
unsigned int FC_startup_fans(void);
unsigned int FC_startup_h2(void);
unsigned int FC_startup_purge(void);
unsigned int FC_startup_charge(void);

unsigned int FC_run(void);

unsigned int FC_shutdown(void);
unsigned int FC_alarm(void);

unsigned int pid_temp_control(void);

#endif /* FUELCELL_FUNCTIONS_H_ */