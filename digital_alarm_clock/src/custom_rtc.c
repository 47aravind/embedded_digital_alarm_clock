/* RTC.c */
/*******************************************************************************
Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include <stdio.h>
#include <machine.h>
#include "platform.h"
#include "custom_rtc.h"
#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
* Global Variables
*******************************************************************************/
char Year1000,Year100,Year10,Year1,Mon10,Mon1,Day10,Day1,Day,Hour10,Hour1,Min10,Min1,Sec10,Sec1;
char buffer[30];

/******************************************************************************
* Function name: rtc_init
* Description  : This function is to initialise RTC time
* Arguments    : none
* Return value : none
******************************************************************************/
void rtc_init(void)
{
	 RTC.RCR2.BIT.START=0; //Write0tostartbitinRCR2 5. 
	 while(RTC.RCR2.BIT.START == 1)
	 {
	 } //Waitforstartbitto clearto06. 
	 
	 RTC.RCR2.BIT.HR24=0; //RTCoperatesin12hourmode7. 
	 RTC.RCR2.BIT.RESET=1; //Write1toResetbitinRCR28. 9. 
	 RTC.RYRCNT.WORD=0x0015; //Setyear,month,dayof 10. 
	 RTC.RMONCNT.BYTE=0x10; //week,date,hour,minute, 11. 
	 RTC.RDAYCNT.BYTE=0x21; //secondto2015Oct21st 12. 
	 RTC.RWKCNT.BYTE=0x02; //Tue12:00:00 13. 
	 RTC.RHRCNT.BYTE=0x12; 
	 RTC.RMINCNT.BYTE=0x00;
	 RTC.RSECCNT.BYTE=0x00; 
	 RTC.RCR2.BIT.START=1; //Setstartbitto1inRCR2 18. 
	 
	 while(RTC.RCR2.BIT.START==0)
	 {
	 } //Waitforstartbittoset to119. 20. //SetcarryinterruptforuselaterwhenreadingtimefromRTC 21. 
	 
	 IEN(RTC,COUNTUP)=0; //DisablecarryinterruptinICU 
	 RTC.RCR1.BIT.CIE = 1;
}

/******************************************************************************
* Function name: DisplayTime
* Description  : This function is to Display RTC time
* Arguments    : none
* Return value : none
******************************************************************************/
void DisplayTime(void)
{ 
	
	
	char DayNames[7] = {'Sun','Mon','Tue','Wed','Thu','Fri','Sat'};
	
	GetTime();
	//lcd_display(LCD_LINE1,"RTCExample");
	sprintf((char*)buffer,"20%d%d%d%d%d%d",Year10,Year1,Mon10,Mon1,Day10,Day1);

	//lcd_display(LCD_LINE2,buffer); 
	//lcd_display(LCD_LINE3,DayNames[Day]); 
	sprintf((char*)buffer,"%d%d:%d%d:%d%d", Hour10, Hour1, Min10, Min1, Sec10, Sec1); 
	lcd_display(LCD_LINE4,buffer); 
	//printf("In display time RTC");
	
} 


/******************************************************************************
* Function name: GetTime
* Description  : This function is to get current RTC time
* Arguments    : none
* Return value : none
******************************************************************************/
void GetTime(void) 
{
	do{ 
	IR(RTC,COUNTUP)=0;//CleartheCarryFlag 5. 
	Year10=RTC.RYRCNT.BIT.YR10; 
	Year1=RTC.RYRCNT.BIT.YR1; 
	Mon10=RTC.RMONCNT.BIT.MON10; 
	Mon1=RTC.RMONCNT.BIT.MON1; 
	Day10=RTC.RDAYCNT.BIT.DATE10; 
	Day1=RTC.RDAYCNT.BIT.DATE1; 
	Day=RTC.RWKCNT.BYTE; 
	Hour10=RTC.RHRCNT.BIT.HR10; 
	Hour1=RTC.RHRCNT.BIT.HR1; 
	Min10=RTC.RMINCNT.BIT.MIN10; 
	Min1=RTC.RMINCNT.BIT.MIN1; 
	Sec10=RTC.RSECCNT.BIT.SEC10; 
	Sec1=RTC.RSECCNT.BIT.SEC1; 
	}while(IR(RTC,COUNTUP)==1);

} 

/******************************************************************************
* Function name: rtc_set_time
* Description  : This function is to set the RTC time
* Return value : none
******************************************************************************/
void rtc_set_time(int hours,int minutes, int seconds)
{
	RTC.RCR2.BIT.START=0; //Write0tostartbitinRCR2 5. 
	 while(RTC.RCR2.BIT.START == 1)
	 {
	 } //Waitforstartbitto clearto06. 
	 
	 RTC.RCR2.BIT.HR24=0; //RTCoperatesin12hourmode7. 
	 RTC.RCR2.BIT.RESET=1; //Write1toResetbitinRCR28. 9. 
	 RTC.RYRCNT.WORD=0x0015; //Setyear,month,dayof 10. 
	 RTC.RMONCNT.BYTE=0x10; //week,date,hour,minute, 11. 
	 RTC.RDAYCNT.BYTE=0x21; //secondto2015Oct21st 12. 
	 RTC.RWKCNT.BYTE=0x02; //Tue12:00:00 13. 
	 RTC.RHRCNT.BYTE=hours; 
	 RTC.RMINCNT.BYTE=minutes;
	 RTC.RSECCNT.BYTE=seconds; 
	 RTC.RCR2.BIT.START=1; //Setstartbitto1inRCR2 18. 
	 
	 while(RTC.RCR2.BIT.START==0)
	 {
	 } //Waitforstartbittoset to119. 20. //SetcarryinterruptforuselaterwhenreadingtimefromRTC 21. 
	 
	 IEN(RTC,COUNTUP)=0; //DisablecarryinterruptinICU 
	 RTC.RCR1.BIT.CIE = 1;
}