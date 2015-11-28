/*
 * FuelCell_ADC.h
 *
 * Created: 2015-11-09 6:26:42 PM
 *  Author: Reegan
 */ 


#ifndef FUELCELL_ADC_H_
#define FUELCELL_ADC_H_



void ADCInit(void);
void StartADC_Sequencers(void);
void ReadADC_Sequencers(void);
int * ReadADC_Sequencer0();


//these are based off of the sequencer order in analog_defs.h
#define MFLOWReading adcvals_0[0] //sequencer 0 adcin8
#define FCTEMP1Reading adcvals_0[1] //seq 0 adcin9
#define FCTEMP2Reading adcvals_0[2] //seq 0 adcin10
#define AMBTEMP1Reading adcvals_0[3] //seq 0 adcin11
#define AMBTEMP2Reading adcvals_0[4] //seq 0 adcin12
#define AMBTEMP3Reading adcvals_0[5] //seq 0 adcin13
#define AMBTEMP0Reading adcvals_0[6] //seq 0 adcin14

//these are based off the sequencer order in analog_defs.h
#define TANKPRESReading adcvals_1[0] //seq 1 adcin1
#define FCPRESReading adcvals_1[1] //seq 1 adcin3
#define CAPCURRReading adcvals_1[2] //seq 1 adcin4



int get_CAPVOLT(void);
int get_FCVOLT(void);
int get_FCCURR(void);

#endif /* FUELCELL_ADC_H_ */