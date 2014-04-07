/*************************************************************************
 * DISCLAIMER *
 * Services performed by FREESCALE in this matter are performed          *
 * AS IS and without any warranty. CUSTOMER retains the final decision   *
 * relative to the total design and functionality of the end product.    *
 * FREESCALE neither guarantees nor will be held liable by CUSTOMER      *
 * for the success of this project. FREESCALE disclaims all warranties,  *
 * express, implied or statutory including, but not limited to,          *
 * implied warranty of merchantability or fitness for a particular       *
 * purpose on any hardware, software ore advise supplied to the project  *
 * by FREESCALE, and or any product resulting from FREESCALE services.   *
 * In no event shall FREESCALE be liable for incidental or consequential *
 * damages arising out of this agreement. CUSTOMER agrees to hold        *
 * FREESCALE harmless against any and all claims demands or actions      *
 * by anyone on account of any damage, or injury, whether commercial,    *
 * contractual, or tortuous, rising directly or indirectly as a result   *
 * of the advise or assistance supplied CUSTOMER in connection with      *
 * product, services or goods supplied under this Agreement.             *
 *************************************************************************/
/*************************************************************************************************
 *
 * File name   : 	Adc.h
 *
 * Description : 	the header file for ADC module
 *
 * History     :
 * 04/01/2007  : Initial Development
 * 
 *************************************************************************************************/

#ifndef _ADC_H
#define _ADC_H

#define MAX_ADC_CH  14

#define ADC_IDLE      0
#define ADC_WAIT_TRIG 1
#define ADC_BUSY      3
#define ADC_FINISH    4


#define SET_RTC_TIME(Time)   { RTCMOD = Time;}
#define SET_RTC_SEC()    { RTCCNT = 0;  RTCSC = 0x9F;} /*Time: s, 0-255*/
#define SET_RTC_HMS()    { RTCCNT = 0;  RTCSC = 0x9D;} /*Time: 100ms,0-10s*/
#define SET_RTC_TMS()    { RTCCNT = 0;  RTCSC = 0x9B;} /*Time: 10ms,0-1s*/
#define SET_RTC_MS()     { RTCCNT = 0;  RTCSC = 0x98;} /*Time: 1ms,0-100*/
#define STOP_RTC()       { RTCSC = 0x80;}


#define SET_TPM_TIMER(Time)  { TPM1CNT = 0; TPM1MOD = Time; TPM1SC = 0x4C;} /*Time = 15(150), Freq = 100K(10KHz);100KHz~100Hz */
#define STOP_TPM()           { if(TPM1SC & 0x18) TPM1SC = 0;}


extern unsigned char ADC_Results_Available;
extern unsigned char Total_Channels;
extern unsigned char Valid_Data_Len;
extern unsigned char Working_Buf;
extern unsigned char ADC_Cvt_Result_Buf0[2*MAX_ADC_CH + 4];
extern unsigned char ADC_Cvt_Result_Buf1[2*MAX_ADC_CH + 4];

extern void ADC_Init(void);
extern unsigned char ADC_Config(char ChHi, char ChLow, char Precision, char Sample_Rate);
extern void Set_Trigger_Type(char Trig_T, char Ch, char Para1, char Para2) ;
extern void Get_ADC_Configuration(char *Buf);
extern char Get_ADC_Result(char *DataBuf);
extern char Get_ADC_Ch_Result(char Ch, char *DataBuf);

extern void Set_Trigger_Type(char Trig_T, char Para1, char Para2, char Ch);
extern char Get_Adc_State(void);
extern char ADC_Start(void);
extern void ADC_Stop(void);


extern void interrupt ADC_ISR (void);
extern void interrupt RTC_ISR(void); 
extern void interrupt TPM1_ISR(void);

#endif