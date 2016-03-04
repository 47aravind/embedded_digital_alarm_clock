/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only 
* intended for use with Renesas products. No other uses are authorized. This 
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE 
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS 
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE 
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer *
* Copyright (C) 2012 Renesas Electronics Corporation. All rights reserved.    
******************************************************************************/
/******************************************************************************
* File Name     : mtu_pwm_mode1_demo.c
* Version       : 1.00
* Device(s)	    : RX63N 
* H/W Platform  : YRDKRX63N
* Description   : Defines MTU functions used for the MTU PWM mode1 demo.
*                 For this demo, the PWM duty cycle is gradually ramped up and 
*                 down to show a range of PWM settings. LEDs are driven at the
*                 PWM rate. This results in a gradual, alternately brightening  
*                 and dimming of the LEDs as the duty cycle ramps.
*******************************************************************************
* History : DD.MM.YYYY     Version     Description
*         : 09.02.2012     1.00        First release
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

#include "platform.h"
#include "mtu_pwm_mode1_demo.h"

/******************************************************************************
Macro definitions
******************************************************************************/
/* Values used to determine the PWM base frequency and duty cycle limits. */
#define PCLK_FRQ     48000000 /* System PCLK Frequency in HZ. */
#define BASE_PWM_FRQ 20000    /* Desired PWM Frequency in HZ. */
#define MAX_DUTY     99       /* Maximum duty cycle percent. */
#define MIN_DUTY     1        /* Minimum duty cycle percent. */

/* Values used by the PWM ramp up/down demo. */
#define UP   true
#define DOWN false
#define RAMP_SKIP    10        /* Something to regulate the demo speed. */  

int Brightness = 0;
/******************************************************************************
Private global variables
******************************************************************************/
/* The count value to set the base frequency. */
const uint16_t g_pwm_base_count = (PCLK_FRQ / BASE_PWM_FRQ);

/* A max count value for the PWM width that must not be exceeded. */
const uint16_t g_max_duty_count = (((PCLK_FRQ / BASE_PWM_FRQ) / 100) * MAX_DUTY);

/* A min count value for the PWM width that must not be exceeded. */
const uint16_t g_min_duty_count = (((PCLK_FRQ / BASE_PWM_FRQ) / 100) * MIN_DUTY);


/******************************************************************************
* Function name : mtu_initialize
* Description   : Set up an MTU timer unit for mode1 PWM operation.
* Arguments     : none
* Return Value  : none
******************************************************************************/
void mtu_initialize (void)
{
    #ifdef PLATFORM_BOARD_RDKRX63N
	SYSTEM.PRCR.WORD = 0xA50B; /* Protect off */
    #endif
   
    MSTP(MTU4) = 0;      /* Cancel MTU peripheral stop state. */

    #ifdef PLATFORM_BOARD_RDKRX63N
	SYSTEM.PRCR.WORD = 0xA500; /* Protect on  */
    #endif

    MTU.TSTR.BIT.CST4 = 0;     /* Stop MTU 4 */

    MTU.TRWER.BIT.RWE = 1; /* Enable read/write access to the write-protected MTU3 and MTU4 registers. */
	MTU.TOER.BIT.OE4A = 1; /* Enable MTIOC4A output. In MTU3 and MTU4, set TOER prior to setting TIOR. */
	MTU.TOER.BIT.OE4B = 1; /* Enable MTIOC4B output. In MTU3 and MTU4, set TOER prior to setting TIOR. */
	//MTU.TOER.BIT.OC1B = 1; // for LCD 
    /* Port E2 Pin Function Select Register (PE2PFS) 
    b7      Reserved: This bit is always read as 0. The write value should always be 0.
    b6      ISEL:     Interrupt Input Function Select, 0 = not used as IRQn input pin
    b5      Reserved: This bit is always read as 0. The write value should always be 0.
    b4:b0   PSEL:     These bits select the peripheral function.
    */
    MPC.PE2PFS.BYTE  = 0x01; /* 1 defines PE2 to be MTIOC4A, with no IRQ. */

    PORTE.PDR.BIT.B2 = 1;    /* Set PE2 as output. */

    PORTE.PMR.BIT.B2 = 1;    /* Set PE2 as peripheral function bit */

    /* Timer Control Register (TCR) 
    b7:b5   CCLR: Counter Clear Source 1 = TCNT cleared by TGRA compare match/input capture 
	                                   2 = TCNT cleared by TGRB compare match/input capture 
    b4:b3   CKEG: Clock Edge           0 = count at rising edge
    b2:b0   TPSC: Time Prescaler       0 = count on PCLK / 1  
    */  
    MTU4.TCR.BYTE = 0x20;  /* Clear on TGRA match. Prescaler = PCLK / 1 setting. */

    /* Timer Mode Register (TMDR)
    b7      ---   Reserved. Always read/write 0.
    b6      BFE - TPUm.TGRE operates normally
    b5      BFB - TPUm.TGRB operates normally
    b4      BFA - TPUm.TGRA operates normally
    b3:b0   MD  - 0 = Normal operation
	              2 = Set PWM mode 1
    */
    MTU4.TMDR.BYTE = 0x02; 
	
    /* Timer I/O Control Register (TIORH)
    b7:b4   IOB - 5 = Ouptut: initial 1, 0 at TGRB compare match
    b3:b0   IOA - MTIOC4A pin function:
                  6 = output: initial 1, then 1 after TGRA match
    */
    MTU4.TIORH.BYTE = 0x56; /* High on TGRA, Low on TGRB. */
  
    /* Timer I/O Control Register (TIORL)
    b7:b4   IOD - 0 = output: none
    b3:b0   IOC - 0 = output: none
    */
    MTU4.TIORL.BYTE = 0x00;
  
    /* Timer General Registers (TGRx)
    *  b15:b0 TGRx either output compare or input capture register.
    *         x may be A, B, C, or D, depending on which registers this MTU has.
	* TGRA is being used as the PWM base frequency counter. Count is cleared on match to this value.
	* TGRB is being used as the PWM pulse width counter. The output is switched when this count is reached.
    */  
    MTU4.TGRA = g_pwm_base_count;      /* MTIOC4A: 1 when TCNT gets to TGRA, 0 when TCNT gets to TGRB */
    MTU4.TGRB = g_pwm_base_count / 2;  /* Starting value 50% duty. */

    /* Timer Interrupt Enable Register (TIER)
        b7      TTEG - A/D conversion start request generation
        b6      reserved
        b5      TCIEU - Underflow interrupts
        b4      TCIEV - Overflow interrupts
        b3      TGIED - TGRD interrupt
        b2      TGIEC - TGRC interrupt
        b1      TGIEB - TGRB interrupt
        b0      TGIEA - TGRA interrupt
    */
    MTU4.TIER.BYTE   = 0x03; /* 0 = no interrupts */
   
    IR (MTU4, TGIA4) = 0;    /* Interrupt reset */
    IPR(MTU4, TGIA4) = 4;    /* Interrupt priority set */
    IEN(MTU4, TGIA4) = 1;    /* Interrupt enable */
	
    IR (MTU4, TGIB4) = 0;    /* Interrupt reset */
    IPR(MTU4, TGIB4) = 4;    /* Interrupt priority set */
    IEN(MTU4, TGIB4) = 1;    /* Interrupt enable */
} /* End of function mtu_initialize(). */


/******************************************************************************
* Function name : mtu_start
* Description   : Starts the MTU counting operation.
* Arguments     : none
* Return Value  : none
******************************************************************************/
void mtu_start (void)
{
    MTU4.TCNT         = 0x0000; /* Clear counter register */
    MTU.TSTR.BIT.CST4 = 1;      /* Start MTU4 */
} /* End of function mtu_start(). */


/******************************************************************************
* Function name : mtu_stop
* Description   : Stops the MTU counting operation.
* Arguments     : none
* Return Value  : none 
******************************************************************************/
void mtu_stop (void)
{
    MTU.TSTR.BIT.CST4 = 0; /* Stop MTU4. */
} /* End of function mtu_stop(). */


/******************************************************************************
* Function name: mtu4_isr
* Description  : Sample ISR for the MTU4 TGRA count match interrupt. 
*                For this demo, the PWM duty cycle is gradually ramped up and 
*                down to show a range of PWM settings. The counter match value
*                for the PWM pulse width is changed here in the ISR at the 
*                completion of each base-frequency period. 
*                LEDs are switched on. The ISR for the TGRB match will
*                switch them back off. This results in a gradual alternately   
*                brightening and dimming of the LEDs.
* Arguments    : none
* Return value : none
******************************************************************************/
#pragma interrupt (mtu4a_isr (vect = VECT(MTU4, TGIA4)))
static void mtu4a_isr (void) 
{
    static bool up_down = UP;
    static uint32_t skip = RAMP_SKIP;

	PORT2.PODR.BIT.B1 = 0;
    //PORTD.PODR.BYTE  = 0x00;   /* Light up all LEDs on port D.*/
    //PORTE.PODR.BYTE &= 0xF0;   /* Light up all LEDs on port E.*/

	if(Brightness == 1)
	{
		Brightness = 0;
		if (MTU4.TGRB < (g_max_duty_count - 200) )
        {
    		MTU4.TGRB = MTU4.TGRB + 100;
		}
	}
	else if(Brightness == 2)
	{	
		Brightness = 0;
		if (MTU4.TGRB > (g_min_duty_count + 200))
        {
			MTU4.TGRB = MTU4.TGRB - 100;   
		}
	}
} /* End of mtu4a_isr. */


/******************************************************************************
* Function name: mtu4_isr
* Description  : Sample ISR for the MTU4 TGRB count match interrupt. 
*                LEDs are switched off. 
* Arguments    : none
* Return value : none
******************************************************************************/
#pragma interrupt (mtu4b_isr (vect = VECT(MTU4, TGIB4)))
static void mtu4b_isr (void) 
{
	PORT2.PODR.BIT.B1 = 1;
    //PORTD.PODR.BYTE  = 0xFF;   /* Turn off all LEDs on port D.*/
    //PORTE.PODR.BYTE |= 0x0F;   /* Turn off all LEDs on port E.*/
} /* End of mtu4b_isr. */


void IncrementBrightness()
{
	Brightness = 2;	
}


void DecrementBrightness()
{
	Brightness = 1;	
}
/*******************************************************************************
* End of file: mtu_pwm_code.c
*******************************************************************************/
