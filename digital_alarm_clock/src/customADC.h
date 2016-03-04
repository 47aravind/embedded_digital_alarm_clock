/***********************************************************************/
/*                                                                     */
/*  FILE        :customADC.h                                           */
/*  DESCRIPTION :Library file for customised ADC                       */
/*                                                                     */
/***********************************************************************/

/*******************************************************************************
Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include "platform.h"

/*******************************************************************************
* Macro Definitions
*******************************************************************************/

/**** Macro Definition for ADC Input Channels ****/
#define AN0 S12AD.ADDR0
#define AN1 S12AD.ADDR1
#define AN2 S12AD.ADDR2
#define AN3 S12AD.ADDR3
#define AN4 S12AD.ADDR4
#define AN5 S12AD.ADDR5
#define AN6 S12AD.ADDR6
#define AN7 S12AD.ADDR7
#define AN8 S12AD.ADDR8
#define AN9 S12AD.ADDR9
#define AN10 S12AD.ADDR10
#define AN11 S12AD.ADDR11
#define AN12 S12AD.ADDR12
#define AN13 S12AD.ADDR13
#define AN14 S12AD.ADDR14
#define AN15 S12AD.ADDR15
#define AN16 S12AD.ADDR16
#define AN17 S12AD.ADDR17
#define AN18 S12AD.ADDR18
#define AN19 S12AD.ADDR19
#define AN20 S12AD.ADDR20


#define n 12       			// Macro Definition for ADC 12 bit value n

#define Vref 3.3			// Macro Definition for V+Ref
#define Vnref 0				// Macro Definition for V-Ref

/*******************************************************************************
* Prototypes for local functions
*******************************************************************************/
void adcInit(void);					// Prototype Declaration for initailization of 12 bit ADC
int adcRead(int channel);			// Prototype Declaration for reading the ADC value
float convertADC(int adcValue);		// Prototype Declaration for converting adcValue to voltage value
int powerOfTwo(int power); 			// Prototype Declaration for calculating the power of 2
