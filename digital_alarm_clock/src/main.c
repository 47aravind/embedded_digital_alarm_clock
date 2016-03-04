/******************************************************************************
* File Name     : main.c
* Version       : 1.0
* Device(s)     : RX63N
* Tool-Chain    : Renesas RX Standard Toolchain 1.0.0
* OS            : None
* H/W Platform  : YRDKRX63N
* Description   : 
*******************************************************************************/

/*******************************************************************************
Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <machine.h>
#include "platform.h"
#include "r_switches.h"
#include "cmt_periodic_multi.h"
#include "custom_rtc.h"
#include "customADC.h"
#include "mtu_pwm_mode1_demo.h"

/*******************************************************************************
* Prototypes for local functions
*******************************************************************************/
void cmt_callback_0(void);
void cmt_callback_1(void);
void DisplayTimeOnLCD(void);
void DisplayTime(void);
void DisplayStopWatchTime(void);
void SetAlarmTime(void);
void ShowAlarmTime(void);
void adcPercentage(void);
void alarm(void);
/*******************************************************************************
* Macro Definitions
*******************************************************************************/

#define TIMER_COUNT_6000MS 562500
#define TIMER_COUNT_1000MS 93750
#define CHANNEL_0   0
#define CHANNEL_1   1
/*******************************************************************************
* Global Variables
*******************************************************************************/
int flagSetTime = 0;			//flag to set the RTC time
int flagDisplayTime = 0;		// flag to display the time
int flagStopWatch = 0;			// flag for stop watch
int flagSetAlarm = 0;			// flag for setting the Alarm

int flagHour = 0;				// flag to set the Hours
int flagMinute = 0;				// flag to set the Minutes
int flagSeconds = 0;			// flag to set the Seconds

int flagStopWatchRunning = 0;	// flag to monitoring stopwatch; 0 corresponds to stop and 1 is running

int hours = 0;					// glabal variable for hours
int minutes = 0;				// glabal variable for minutes
int seconds = 0;				// glabal variable for seconds

int switch1PressCount = 0;		// counter to count the switch 1 press count

int stopWatchHours = 0;			// flag for stop watch hours
int stopWatchMinutes = 0;		// flag for stop watch minutes
int stopWatchSeconds = 0;		// flag for stop watch seconds

int alarmHours = 0;				// global variable for alarm hours
int alarmMinutes = 0;			// global variable for alarm minutes
int alarmSeconds = 0;			// global variable for alarm seconds

int flagAlarmHours = 0;			// flag for alarm hours
int flagAlarmMinutes = 0;		// flag for alarm minutes
int flagAlarmSeconds = 0;		// flag for alarm seconds

int flagAlarmIsOn = 0; 			// flag to alarm is ON or OFF; 0 indicates no alkarm is set and vice versa

int backLightCounter = 0;		// backlight counter global variable	

int volumeTimeCounter = 0;		// global variable for counting the volume time
int volumePercentage = 0;		// global variable for counting the volume percentage

int flagLCDBrightness = 0;		// flag for LCD brightness	
/******************************************************************************
* Function name: main
* Description  : Main program function
* Arguments    : none
* Return value : none
******************************************************************************/
void main(void)
{
	int i=0,j=0;
	
	rtc_init();					// RTC init
	PORT0.PDR.BIT.B5 = 1; 		
	PORT0.PMR.BIT.B5 = 0;
	 
	PORTA.PDR.BIT.B2 = 1;		// AMP_SHDN pin as output
	PORTA.PMR.BIT.B2 = 0;  
	PORTA.PODR.BIT.B2 =1;		// Enabling speaker through AMP_SHDN
	
	PORTC.PDR.BIT.B1 = 1;		// Speaker Gain Pin as Output
	PORTC.PMR.BIT.B1 = 0;  
	PORTC.PODR.BIT.B1 =0;		// Selecting 6dB gain for speaker
		
	PORT1.PDR.BIT.B7 = 1;		// Selecting Audio-R pin as Output for speaker 
	PORT1.PMR.BIT.B7 = 0;		
	
	    /* Used to pace toggling of the LED */
	
	PORT2.PDR.BIT.B1 = 1;		// LCD backlight Enable pin
		
		
    uint32_t led_counter;
    
    
    adcInit();				// Initailising ADC 
	mtu_initialize();   /* Set up the MTU. */

	/* Initialize LCD */
	lcd_initialize();
    
	
    
    mtu_start();        /* Start the PWM demo.*/
	
    /* Clear LCD */
    lcd_clear();
    
    /* Display message on LCD */
    //lcd_display(LCD_LINE1, "  RENESAS   ");
    //lcd_display(LCD_LINE2, " YRDKRX63N  ");
	lcd_display(LCD_LINE3, "Display Time");
	flagDisplayTime = 1;		// making display time flag 1
    /* 
        STDOUT is routed through the virtual console window tunneled through the JTAG debugger.
        Open the console window in HEW to see the output 
    */
    printf("This is the debug console\r\n");
    
    /* The three pushbuttons on the YRDK board are tied to interrupt lines, set them up here */
    R_SWITCHES_Init();			// Switches Init
	
    cmt_init();					// Cmt timer Init
	cmt_callback_set(CHANNEL_0, &cmt_callback_0);		// setting Callback for channetl 0
	cmt_callback_set(CHANNEL_1, &cmt_callback_1);		// setting Callback for channetl 1
    /* This is the main loop.  It does nothing but toggle LED4 periodically */
	cmt_start(CHANNEL_0,TIMER_COUNT_1000MS);			// channel 0 start for controlling the LCD backlight and to turn it OFF once the timer is over
	
	rtc_init();											// Initailising RTC
    while (1)
    {
		
		adcPercentage();								// Calculate the ADC percentage for ALARM volume
		
		if(flagSetTime)									// if the flag set time is high, allow user to set the time 
		{
			DisplayTimeOnLCD();
		}
		
		if(flagDisplayTime)								// if flag display time is high Display the time on LCD
		{	
			//continue;
			
			DisplayTime();
			//lcd_display(LCD_LINE3, "Yooo");
		}
		
		if(flagStopWatch)								// if flag stop watch is high show stop watch time on LCD
		{
			DisplayStopWatchTime();			
		}
		
		if(flagSetAlarm)								// if flag set Alarm is high, alllow user to set the alarm
		{
			SetAlarmTime();
		}
		
		if(flagAlarmIsOn)								// if the alarm is set by the user check the alarm time and turn on the speaker if alarm condition meets up
		{
			printf("cheking alarm  ");
			printf("%c\n",RTC.RHRCNT.BYTE );
			if(RTC.RHRCNT.BIT.HR10 == (char)(alarmHours/10) && RTC.RHRCNT.BIT.HR1 == (char)(alarmHours%10) && RTC.RMINCNT.BIT.MIN10 == (char)(alarmMinutes/10) && RTC.RMINCNT.BIT.MIN1 == (char)(alarmMinutes%10))
			{
				printf("Alarmmmmmm");
				
				lcd_display(LCD_LINE1, "ALARM");
				
				alarm(volumePercentage*10);
				
				//PORTA.PODR.BIT.B2 = 0; 
				//PORT0.PODR.BIT.B5 = 1;
				//PORT1.PODR.BIT.B7 = 1;
			}
		}
		
		for(i =0;i<50000;i++)
			for(j=0;j<100;j++)
				;		

		
    /*    for (led_counter = 0; led_counter < 1000000; led_counter++)
        {
        }
        LED4 = ~LED4;
		*/
    }
} /* End of function main() */


/******************************************************************************
* Function name: alarm
* Description  : This function will give square wave input to speaker when the alarm condition is true
* Arguments    : int time
* Return value : none
******************************************************************************/
void alarm(int time)
{
	int i = 0;
	int j = 0;
	
	for(i=0;i<time;i++){
		PORT1.PODR.BIT.B7=1;
		PORTC.PODR.BIT.B1 =0;
		for(j=0;j<10000;j++)
		;
		PORT1.PODR.BIT.B7=0;
		PORTC.PODR.BIT.B1 =1;
		for(j=0;j<10000;j++)
		;
	}
	
	for(i=0;i<time;i++){
		PORT1.PODR.BIT.B7=1;
		for(j=0;j<10000;j++)
		;
		PORT1.PODR.BIT.B7=1;
		for(j=0;j<10000;j++)
		;
	}
	
}

/******************************************************************************
* Function name: adcPercentage
* Description  : This function is to calculate potentiometer reading into percentage and showing it on lCD
* Arguments    : none
* Return value : none
******************************************************************************/
void adcPercentage(void)
{
	int maxAdcValue = 4095;
	int adcCurrentValue = 0;
	int adcPercentage = 0;
	char ADC_OUT;
	
	adcCurrentValue = adcRead(AN2);
	adcCurrentValue = adcCurrentValue * 100;
	adcPercentage = adcCurrentValue/maxAdcValue;

	if(volumePercentage != adcPercentage)
	{
		volumeTimeCounter = 0;
	}
		
	if(volumePercentage != adcPercentage || volumeTimeCounter < 40)
	{
		volumePercentage = adcPercentage;
		volumeTimeCounter++;
		
		sprintf(ADC_OUT,"%d",adcPercentage);
		lcd_display(LCD_LINE2, ADC_OUT);
	}
	else
	{
		lcd_display(LCD_LINE2, " ");
	}
	

}

/******************************************************************************
* Function name: SetAlarmTime
* Description  : This function allows user to set the Alarm and also shows a blinking animation on LCD
* Arguments    : none
* Return value : none
******************************************************************************/
void SetAlarmTime()
{
	char charHours,charMinutes,charSeconds;
	int i=0,j=0;
	char time[12];

	DisplayTime();

	printf("In display set time..");
	sprintf(time,"%02d:%02d:%02d",alarmHours,alarmMinutes,alarmSeconds);	
	lcd_display(LCD_LINE5, time);
	
	for(i =0;i<50000;i++)
		for(j=0;j<100;j++)
			;
	if(flagAlarmHours)			
		sprintf(time,"%s:%02d:%02d","  ",alarmMinutes,alarmSeconds);	
		lcd_display(LCD_LINE5, time);
	
		for(i =0;i<50000;i++)
			for(j=0;j<100;j++)
				;		

	if(flagAlarmMinutes)			
		sprintf(time,"%02d:%s:%02d",alarmHours,"  ",alarmSeconds);	
		lcd_display(LCD_LINE5, time);
	
		for(i =0;i<50000;i++)
			for(j=0;j<100;j++)
				;		
	if(flagAlarmSeconds)			
		sprintf(time,"%02d:%02d:%s",alarmHours,alarmMinutes,"  ");	
		lcd_display(LCD_LINE5, time);
	
		for(i =0;i<50000;i++)
			for(j=0;j<100;j++)
				;		
	
}

/******************************************************************************
* Function name: DisplayStopWatchTime
* Description  : Function to display Stop Watch time
* Arguments    : none
* Return value : none
******************************************************************************/
void DisplayStopWatchTime(void)
{
	char time[12];
	
	sprintf(time,"%02d:%02d:%02d",stopWatchHours,stopWatchMinutes,stopWatchSeconds);	
	lcd_display(LCD_LINE4, time);
		
}

/******************************************************************************
* Function name: DisplayTimeOnLCD
* Description  : Function to display time on LCD
* Arguments    : none
* Return value : none
******************************************************************************/
void DisplayTimeOnLCD(void)
{
	char charHours,charMinutes,charSeconds;
	int i=0,j=0;
	char time[12];

	/*
	sprintf(charHours,"%d",hours);	
	lcd_display(LCD_LINE4, charHours);
	
	sprintf(charMinutes,"%d",minutes);	
	lcd_display(LCD_LINE5, charMinutes);
	
	sprintf(charSeconds,"%d",seconds);	
	lcd_display(LCD_LINE6, charSeconds);
	*/
	printf("In display set time..");
	sprintf(time,"%02d:%02d:%02d",hours,minutes,seconds);	
	lcd_display(LCD_LINE7, time);
	
	for(i =0;i<50000;i++)
		for(j=0;j<100;j++)
			;
	if(flagHour)			
		sprintf(time,"%s:%02d:%02d","  ",minutes,seconds);	
		lcd_display(LCD_LINE7, time);
	
		for(i =0;i<50000;i++)
			for(j=0;j<100;j++)
				;		

	if(flagMinute)			
		sprintf(time,"%02d:%s:%02d",hours,"  ",seconds);	
		lcd_display(LCD_LINE7, time);
	
		for(i =0;i<50000;i++)
			for(j=0;j<100;j++)
				;		
	if(flagSeconds)			
		sprintf(time,"%02d:%02d:%s",hours,minutes,"  ");	
		lcd_display(LCD_LINE7, time);
	
		for(i =0;i<50000;i++)
			for(j=0;j<100;j++)
				;		
	
}

/******************************************************************************
* Function name: ShowAlarmTime
* Description  : Function to show alarm time on LCD
* Arguments    : none
* Return value : none
******************************************************************************/
void ShowAlarmTime(void)
{
	char time[12];
	
		sprintf(time,"%02d:%02d:%s",alarmHours,alarmMinutes,alarmSeconds);	
		lcd_display(LCD_LINE5, time);
	
}

/******************************************************************************
* Function name: sw1_callback
* Description  : Callback function that is executed when SW1 is pressed.
*                Called by sw1_isr in r_switches.c
* Arguments    : none
* Return value : none
******************************************************************************/
void sw1_callback(void)
{
	
	int i=0,j=0;
	
	mtu_start();
	PORT2.PODR.BIT.B1 = 1;	
	backLightCounter = 0;


	printf("Switch 1 pressed..");
	/* Check if the Display time flag is high if high transfer the clock to next mode or if the switch is held low for 5 sec then set the flag and allow user to Set the RTC time */
	if(flagDisplayTime)						
	{
		printf("Inside flag display time..\n");
		flagSetTime = 0;
		flagDisplayTime = 0;
		//flagStopWatch = 1;
		flagStopWatch = 0;
		flagSetAlarm = 0;
		PORT4.PDR.BIT.B0 = 0;
		
		for(i = 0; i<10000; i++)
		{
			for(j = 0; j<1000; j++)
			{
				if(PORT4.PIDR.BIT.B0 == 0)	
				{
					flagSetTime = 1;
					flagHour = 1;
					//continue;
				}
				else
				{
					flagSetTime = 0;
					flagStopWatch = 1;
					break;
				}	
			
			}		
			if(flagStopWatch == 1)
				break;
		}
		
		printf("after for loop inside display time..\n");
		
		if(flagStopWatch == 1)
		{
			printf("flag stop watch if before display..\n");
			lcd_display(LCD_LINE3, "Stop Watch");
			printf("flag stop watch if..\n");
		}
		else
		{
			//lcd_clear();
			lcd_display(LCD_LINE3, "Set Time");
		}
	}
	else if(flagStopWatch)				// if stop watch flag is high and switch is pressed then go to the Set alarm mode
	{
		flagSetTime = 0;
		flagDisplayTime = 0;
		flagStopWatch = 0;
		flagSetAlarm = 1;
		flagAlarmHours = 1;
		lcd_display(LCD_LINE3, "Set Alarm");
		ShowAlarmTime();
	}
	else if(flagSetTime)				// if set time flag is high and switch is pressed then toggle in between setting Hours, Minutes and seconds and once the time is set go to display time mode
	{
		if(flagHour)
		{
			flagHour = 0;
			flagMinute = 1;
			flagSeconds = 0;
		}
		else if(flagMinute)
		{
			flagHour = 0;
			flagMinute = 0;
			flagSeconds = 1;
		}
		else if(flagSeconds)
		{
			flagHour = 0;
			flagMinute = 0;
			flagSeconds = 0;
			flagDisplayTime = 1;
			flagSetTime = 0;
			rtc_set_time(hours,minutes,seconds);
			//DisplayTime();
		}
	}
	else if(flagSetAlarm)  			// if set alarm flag is high and switch is pressed then toggle in between setting alarm Hours, alarm Minutes and alarm seconds and once the time is set go to brightness mode
	{
		if(flagAlarmHours)
		{
			flagAlarmHours = 0;
			flagAlarmMinutes = 1;
			flagSeconds = 0;
		}
		else if(flagAlarmMinutes)
		{
			flagAlarmHours = 0;
			flagAlarmMinutes = 0;
			flagAlarmSeconds = 1;
		}
		else if(flagAlarmSeconds)
		{
			flagAlarmHours = 0;
			flagAlarmMinutes = 0;
			flagAlarmSeconds = 0;
			
			flagLCDBrightness =1;
			flagDisplayTime = 0;
			flagStopWatch = 0;
			flagSetAlarm = 0;
			lcd_display(LCD_LINE3, "Brightness");
		
			lcd_display(LCD_LINE1, "A");
			flagAlarmIsOn = 1;
			//flagDisplayTime = 1;
			//flagSetTime = 0;
			//rtc_set_time(hours,minutes,seconds);
			//DisplayTime();
		}
	}
	else if(flagLCDBrightness)				// if LCD brightness flag is high and switch is pressed then go to display time mode
	{
			flagLCDBrightness =0;
			flagDisplayTime = 1;
			lcd_display(LCD_LINE3, "Display Time");
		
	}
	else
	{
		
	}
	
	for(i=0;i<500;i++)
		for(j=0;j<500;j++)
			;
    //nop(); /* Add your code here. Note: this is executing inside an ISR. */
} /* End of function sw1_callback() */

/******************************************************************************
* Function name: cmt_callback_1
* Description  : Callback function that is executed when cmt timer 1 is over
* Arguments    : none
* Return value : none
******************************************************************************/
void cmt_callback_1(void)
{
	/* This cmt timer is used for stop watch time calculation */
	stopWatchSeconds++;
	
	if(stopWatchSeconds>59)
	{
		stopWatchSeconds = 0;
		stopWatchMinutes++;
	}
	if(stopWatchMinutes>59)
	{
		stopWatchMinutes = 0;
		stopWatchHours++;
	}

}

/******************************************************************************
* Function name: cmt_callback_0
* Description  : Callback function that is executed when cmt timer 0 is over
* Arguments    : none
* Return value : none
******************************************************************************/
void cmt_callback_0(void)
{
	/* This cmt timer is used for monitoring and turning the LCD backlight */
	backLightCounter++;
	
	
	if(backLightCounter>60)
	{
		mtu_stop();
		PORT2.PODR.BIT.B1 = 0;
	}
	
}

/******************************************************************************
* Function name: sw2_callback
* Description  : Callback function that is executed when SW2 is pressed.
*                Called by sw2_isr in r_switches.c
* Arguments    : none
* Return value : none
******************************************************************************/
void sw2_callback(void)
{
	mtu_start();
	PORT2.PODR.BIT.B1 = 1;	
	backLightCounter = 0;
	
	if(flagHour && flagSetTime)					// if hour flag is high and flag set time is high then increment the hours count if it exceeds 23 then set it to 0 
	{
		hours++;
		if(hours>23)
			hours = 0;
	}
	else if(flagMinute && flagSetTime)			// if minutes flag is high and flag set time is high then increment the minutes count if it exceeds 59 then set it to 0
	{
		minutes++;
		if(minutes>59)
			minutes = 0;
	}
	else if(flagSeconds && flagSetTime)			// if seconds flag is high and flag set time is high then increment the seconds count if it exceeds 59 then set it to 0
	{
		seconds++;
		if(seconds>59)
			seconds = 0;	
	}
	else if(flagStopWatch)						//  if flag stopwatch is high then start or stop the StopWatch
	{
		if(flagStopWatchRunning == 0)
		{
			flagStopWatchRunning = 1;
			cmt_start(CHANNEL_1,TIMER_COUNT_1000MS);
		}
		else if(flagStopWatchRunning == 1)
		{
			flagStopWatchRunning = 0;
			cmt_stop(CHANNEL_1);
		}
	}
	else if(flagSetAlarm && flagAlarmHours)		// if alarm hour flag is high and  flag alarm set time is high then increment the hours count if it exceeds 23 then set it to 0 
	{
		alarmHours++;
		if(alarmHours>23)
			alarmHours = 0;		
	}
	else if(flagSetAlarm && flagAlarmMinutes)  // if alarm minutes flag is high and  flag alarm set time is high then increment the minutes count if it exceeds 59 then set it to 0 
	{
		alarmMinutes++;
		if(alarmMinutes>59)
			alarmMinutes = 0;
	}
	else if(flagSetAlarm && flagAlarmSeconds)	// if alarm seconds flag is high and  flag alarm set time is high then increment the seconds count if it exceeds 59 then set it to 0 
	{
		alarmSeconds++;
		if(alarmSeconds>59)
			alarmSeconds = 0;
	}
	else if(flagDisplayTime)					// if display time is high, Turn on or Turn off the alarm
	{
		if(flagAlarmIsOn)
		{
			lcd_display(LCD_LINE1, " ");
			flagAlarmIsOn = 0;
		}
		else
		{
			lcd_display(LCD_LINE1, "A");
			flagAlarmIsOn = 1;	
		}
	}
	else if(flagLCDBrightness)					// If LCD brightness flag is high increment the brightness
	{
		IncrementBrightness();
	}
    //nop(); /* Add your code here. Note: this is executing inside an ISR. */
} /* End of function sw2_callback() */


/******************************************************************************
* Function name: sw3_callback
* Description  : Callback function that is executed when SW3 is pressed.
*                Called by sw3_isr in r_switches.c
* Arguments    : none
* Return value : none
******************************************************************************/
void sw3_callback(void)
{
	mtu_start();
	PORT2.PODR.BIT.B1 = 1;	
	backLightCounter = 0;

	if(flagHour && flagSetTime)					// if hour flag is high and flag set time is high then decremetn the hours count 
	{
		hours--;
		if(hours<0)
			hours = 23;
	}
	else if(flagMinute && flagSetTime)			// if minutes flag is high and flag set time is high then decrement the minutes count  
	{
		minutes--;
		if(minutes<0)
			minutes = 59;
	}
	else if(flagSeconds && flagSetTime)			// if seconds flag is high and flag set time is high then decrement the seconds count  	
	{
		seconds--;
		if(seconds<0)
			seconds = 59;	
	}
	else if(flagStopWatch)					// if stop watch flag is high and switch is pressed clear the stopwatch time
	{
		stopWatchHours = 0;
		stopWatchMinutes = 0;
		stopWatchSeconds = 0;
		
		flagStopWatchRunning = 0;
		cmt_stop(CHANNEL_1);
	}
	else if(flagSetAlarm && flagAlarmHours) // if alarm hours flag is high and  flag alarm set time is high then decrement the hours count 
	{
		alarmHours--;
		if(alarmHours < 0)
			alarmHours = 23;		
	}
	else if(flagSetAlarm && flagAlarmMinutes) // if alarm minutes flag is high and  flag alarm set time is high then decrement the minutes count 
	{
		alarmMinutes--;
		if(alarmMinutes < 0)
			alarmMinutes = 59;
	}
	else if(flagSetAlarm && flagAlarmSeconds)  // if alarm seconds flag is high and  flag alarm set time is high then decrement the seconds count 
	{
		alarmSeconds--;
		if(alarmSeconds< 0)
			alarmSeconds = 59;
	}
	else if(flagLCDBrightness)				// If LCD brightness flag is high decrement the brightness
	{
		DecrementBrightness();	
	}
    //nop(); /* Add your code here. Note: this is executing inside an ISR. */
} /* End of function sw3_callback() */

