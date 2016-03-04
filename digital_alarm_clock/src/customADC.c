/***********************************************************************/
/*                                                                     */
/*  FILE        :customADC.c                                           */
/*  DESCRIPTION :Library file for customised ADC                       */
/*                                                                     */
/***********************************************************************/

/*******************************************************************************
Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include "stdio.h"
#include "customADC.h"
#include "platform.h"

/*******************************************************************************
* Macro Definitions
*******************************************************************************/


/***********************************************************************************************************************
* Function Name: adcInit
* Description  : Initializes the 12 bit ADC.
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
void adcInit(void){
	SYSTEM.MSTPCRA.BIT.MSTPA17 = 0;		// 12-bit ADC is selected
	S12AD.ADCSR.BYTE = 0x0C;			// Control register is set up with software trigger enabled,PCLK is chosen with 48M, Interrupt is not enabled and Single scan mode has been enabled
	S12AD.ADANS0.WORD = 0x00FF;   		// Channel 0-7 are selected for ADC conversion
	S12AD.ADCER.BIT.ACE = 1;			// Automatic clearing of ADDRn is enabled 
	S12AD.ADCER.BIT.ADRFMT = 0;			// Right Alignment of ADDRn is done
}


/***********************************************************************************************************************
* Function Name: adcRead
* Description  : Reads the value from 12 bit ADC.
* Arguments    : int channel
* Return Value : float adc_result
***********************************************************************************************************************/
int adcRead(int channel){

	int adc_result = 0;
	
	S12AD.ADCSR.BIT.ADST = 1;					// 12 bit start of conversion
	S12AD.ADANS0.WORD = 0x0004; 
	while(1){
		
		if(S12AD.ADCSR.BIT.ADST == 0){			// checking for end of conversion
	
    		adc_result = channel & 0x0FFF;       // Reading the ADC value on channel and masking the right 12 bits of the register
			
			
    		return adc_result;
			
		}
	}
}


/***********************************************************************************************************************
* Function Name: powerOfTwo
* Description  : This function returns the value for power of two
* Arguments    : int power
* Return Value : int resultPower
***********************************************************************************************************************/

int powerOfTwo(int power){
	int i =0;
	int resultPower =1;
	for(i=0; i<power; i++){
		
		resultPower = resultPower*2;
		
	}
	return resultPower;
}


/***********************************************************************************************************************
* Function Name: convertADC
* Description  : This function converts digitalized ADC value into ADC voltage
* Arguments    : int adcValue
* Return Value : float vinResult
***********************************************************************************************************************/

float convertADC(int adcValue){
	
	float vinResult = 0;
	
	int numberOfSteps = powerOfTwo(n);
	
	vinResult = (((Vref-Vnref) * adcValue)/(numberOfSteps - 1)) + Vnref;	// calculating the ADC input voltage
	
	return vinResult;
}

