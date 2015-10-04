#ifndef FuelCell_Inputs
#define FuelCell_Inputs

void InputsInit(void);
unsigned char ReadSTART(void);
unsigned char ReadSYSOK(void);
unsigned int ReadTANKPRES(void);
unsigned int ReadFCPRES(void);
unsigned int ReadFAN_TAC(void);
unsigned int ReadFCCURR(void);
unsigned int ReadFCVOLT(void);
unsigned char ReadFCCON(void);
unsigned int ReadCAPCURR(void);
unsigned char ReadCAPCON(void);
unsigned int ReadCAPVOLT(void);
unsigned char ReadRESCON(void);
unsigned int ReadAMBTEMP1(void);
unsigned int ReadAMBTEMP2(void);
unsigned int ReadAMBTEMP3(void);
unsigned int ReadAMBTEMP4(void);
unsigned int ReadFCTEMP1(void);
unsigned int ReadFCTEMP2(void);
unsigned int ReadMFLOW(void);



#endif