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
 * File name   : Adc.c
 * Description : This software defines the routines for ADC
 *               The ADC module is used to sample the potentiomemter on demo board
 *               which is used to control the mouse shift direction 
 *
 * History     :
 * 04/01/07  : Initial Development
 * 
 *************************************************************************************************/
#include <MC9S08JM60.h>
#include <string.h>
#include "typedef.h"
#include "adc.h"

/* Local variable definition*/
unsigned char Samp_Rate;            /*0: 1Hz, 1: 10Hz, 2:100Hz, 3:1KHz, 4: 10KHz, 5:100KHz, */
unsigned char Trig_Type;            /*0: SW, 1: HW, 2: Compare*/
unsigned char Trig_Para1;           /*HW: Delay time;  Comp: Comp channel*/
unsigned char Trig_Para2;           /*Comp: 0: Greater, 1: Less than */
unsigned char Trig_Para3;           /*Comp: Comp: Level, HighVol* x%  */

unsigned char ADC_State;
unsigned char Current_Channel;
unsigned char Work_Channel[MAX_ADC_CH];
unsigned char Is_ADC_Configured = 0;

DWORD Time_Stamp;

/*global variable definition*/
unsigned char Total_Channels;
unsigned char Valid_Data_Len = 0;
unsigned char ADC_Results_Available = 0;

unsigned char Working_Buf = 0;
unsigned char ADC_Cvt_Result_Buf0[2*MAX_ADC_CH + 4]; /*for conversion results of all channels and timestamp*/
unsigned char ADC_Cvt_Result_Buf1[2*MAX_ADC_CH + 4]; 

/* globe function definition*/   
void ADC_Init(void);
unsigned char ADC_Config(char ChHi, char ChLow, char Precision, char Sample_Rate);
void Get_ADC_Configuration(char *Buf);
char Get_ADC_Result(char *DataBuf);
char Get_ADC_Ch_Result(char Ch, char *DataBuf);

void Set_Trigger_Type(char Trig_T, char Ch, char Para1, char Para2) ;

char Get_Adc_State(void);

char ADC_Start(void);
void ADC_Stop(void);

void interrupt ADC_ISR (void);
void interrupt RTC_ISR(void); 
void interrupt TPM1_ISR(void);

/**********************************************************************************************
 * ADC_Init: This function initilizes the ADC module
 *
 * Parameters:      none
 *
 * Subfunctions:    none.
 *
 * Return:          void
 *********************************************************************************************/ 
void ADC_Init(void)
{
     char i;
     
     ADCCFG = 0x40;     /*  busclk, Div by 4,ADCK = 6MHz*/
                        /*  0b0000000 0
                         *    ||||||| |__ bit0,1: ADICLK : input clock select   
                         *    |||||||_|
                         *    ||||||_____ bit2,3: MODE :  Conversion Mode selection
                         *    |||||_|  
                         *    ||||______ bit4:    ADLSMP: long sample time configuration
                         *    |||_______ bit5,6 : ADIV:   Clock Divide Select 
                         *    ||______| 
                         *    |_________ bit7:    ADLPC:  Low power configuration 
                         */
     
     ADCSC2 = 0x00;  
                        /*  0b00000000
                         *    ||||||||__ bit0:   
                         *    |||||||___ bit1: 
                         *    ||||||____ bit2: 
                         *    |||||_____ bit3: 
                         *    ||||______ bit4: ACFGT: Compare function greater than enable
                         *    |||_______ bit5: ACFE : Compare enable
                         *    ||________ bit6: ADTRG: Conversion trigger select
                         *    |_________ bit7: ADACT: Convert active
                         */

   
    
    APCTL1 = 0x00;   /*disable all ADC ports*/
    APCTL2 = 0x00;
   
    
    Is_ADC_Configured = 0;
    Current_Channel = 0; 
    Total_Channels = 0;
    Trig_Type = 0; 
    
    Time_Stamp._dword = 0;
    
    for(i= 0; i< MAX_ADC_CH; i++)
      Work_Channel[i] = 0; 
      
    return;
}


/**********************************************************************************************
 * ADC_Config: This function configures the ADC module
 *
 * Parameters:      ChHi:   the high byte of channel option (which channel is enabled)
 *                  ChLow:  the low byte of channel option
 *                  Precision:  0: 8bit 1:10bit 2:12bit
 *                  Sample_Rate:       sample rate
 *                  trigger type:      0: software trigger, 1: hardware trigger 
 *
 * Return:          0 : failed 1: successed 2: wrong parameters
 *********************************************************************************************/ 
unsigned char ADC_Config(char ChHi, char ChLow, char Precision, char Sample_Rate) 
{
   char i;
   
   if(ADC_State == ADC_IDLE) 
   {
       APCTL1 = ChLow;
       APCTL2 = (ChHi & 0x0F);
       
       Current_Channel = 0;
       Total_Channels = 0;
       
       for(i = 0; i < 8 ; i++) 
       {
         if(ChLow & (1 << i)) 
         {
            Work_Channel[Current_Channel] = (i | 0x40);
            Current_Channel ++;
            Total_Channels ++;
         }
       }
       
       for(i = 0; i < 6 ; i++) 
       {
         if(ChHi & (1 << i)) 
         {
            Work_Channel[Current_Channel] = (i + 8);
            if(Work_Channel[Current_Channel] == 0x0C)
               Work_Channel[Current_Channel] = 0x1A;
            else 
            {
               if(Work_Channel[Current_Channel] == 0x0D)
                 Work_Channel[Current_Channel] = 0x1B;
            }
            
            Work_Channel[Current_Channel] |= 0x40;
            Current_Channel ++;
            Total_Channels ++;
         }
       }
       
       Valid_Data_Len =  2*Total_Channels + 4;
       
       if(Precision == 2)             /*12bit*/
          ADCCFG_MODE = 0b01;
       else
        if(Precision == 1)            /*10bit*/
          ADCCFG_MODE = 0b10;
        else
          ADCCFG_MODE = 0b00;         /*8bit*/
       
       
       if(Sample_Rate < 0x13)
          Samp_Rate = Sample_Rate;
       else 
          return 2;
       
      
       Is_ADC_Configured = 1;
       return 1;
   } 
   else
      return 0;
}

/**********************************************************************************************
 * Get_ADC_Configuration: This function returns the configuration of ADC module
 *
 * Parameters:      Buf : point to the location which save the configuration
 *
 * Return:          
 *********************************************************************************************/ 
void Get_ADC_Configuration(char *Buf) 
{
    char i;
    char Channel;
    int Tmp;
    
    for(i=0; i < Total_Channels; i++) 
    {
      Channel =  Work_Channel[i] & 0x1F;
      if(Channel < 0x0C)
        Tmp |= (1<< Work_Channel[i]);
      else 
      {
        if(Channel == 0x1A)
          Tmp |= (1 << 0x0C);
        else
          if(Channel == 0x1B)
            Tmp |= (1 << 0x0D);
      }
    }
      
    *Buf = (char)(Tmp >> 8);
    *(Buf+1) = (char)Tmp;
    
    if(ADCCFG_MODE == 0b01)
      *(Buf+2) = 0x02;          /*12bit*/
    else
      if(ADCCFG_MODE == 0b10)
        *(Buf+2) = 0x01;        /*10bit*/
      else
        *(Buf+2) = 0x00;        /*8bit*/
      
    *(Buf+3) = Samp_Rate;
    
    *(Buf+4) = Trig_Type;
    *(Buf+5) = Trig_Para1;
    *(Buf+6) = Trig_Para1;
    *(Buf+7) = Trig_Para1;
     
    
   return; 
}

/**********************************************************************************************
 * Get_ADC_Configuration: This function returns the state of ADC module
 *
 * Parameters:      None
 *
 * Return:          the state of ADC module
 *********************************************************************************************/ 
char Get_Adc_State(void)
{
   if(Is_ADC_Configured)
    return (ADC_State | 0x80);
   else
    return ADC_State ;
}


/**********************************************************************************************
 * Set_Trigger_Type: This function sets the trigger type of ADC module
 *
 * Parameters:      Trig_T  : Trigger type 0: Software, 1: Hardware, 2:Level
 *                  Para1:    The delay for hardware trigger, 1-256s
 *                            The option of greater or less for Level trigger
 *                            0 : Great than 1: Less than
 *                            No use for software trigger
 *                  Para2:    The percentage of highest voltage for level trigger (0 - 100)
 *                            No use for software and hardware trigger
 * Return:          
 *********************************************************************************************/ 
void Set_Trigger_Type(char Trig_T, char Para1, char Para2, char Para3) 
{
  Trig_Type = Trig_T & 0x03;
  Trig_Para1 = Para1;
  Trig_Para2 = Para2;
  Trig_Para3 = Para3;
  return;
}


/**********************************************************************************************
 * Config_Samp_Clk: This function configs sample clk
 *
 * Parameters:      
 * Return:          
 *********************************************************************************************/ 
void Config_Samp_Clk(void) 
{
    if(Samp_Rate < 12) 
   {
     switch(Samp_Rate%3) 
     {
      case 0:
        SET_RTC_TIME(100);
      break;
      
      case 1:
        SET_RTC_TIME(50);
      break;
      
      case 2:
        SET_RTC_TIME(20);
      break;
      
      default:
      break;
     }
     
     switch(Samp_Rate/3) 
     {
      case 0:
        SET_RTC_SEC();
      break;
      
      case 1:
        SET_RTC_HMS();
      break;
      
      case 2:
        SET_RTC_TMS();
      break;
      
      case 3:
        SET_RTC_MS();
      break;
      
      default:
      break;
     }
   } 
   else 
   {
     switch(Samp_Rate) 
     {
      case 12:                /*100Hz, 0.01s*/
        SET_TPM_TIMER(15000);
      break;
      
      case 13:                /*200Hz, 0x005s*/
        SET_TPM_TIMER(7500);
      break;
      
      case 14:                /*500Hz, 0x002s*/
        SET_TPM_TIMER(3000);
      break;
      
      case 15:                /*1000Hz, 0x001s*/
        SET_TPM_TIMER(1500);
      break;
      
      case 16:                /*2000Hz, 0x0005s*/
        SET_TPM_TIMER(750);
      break;
      
      case 17:                /*5000Hz, 0x0002s*/
        SET_TPM_TIMER(300);
      break;
      
      case 18:                /*10000Hz, 0x0001s*/
        SET_TPM_TIMER(150);
      break;
      
      default:
      break;
     }
   }
   return;
}

/**********************************************************************************************
 * Get_ADC_Result: This function is used to fetch the all ADC conversion result
 *
 * Parameters:      Buf : point to the location which save the result
 *
 * Return:     0: failed, the conversion is not finished 
 *             1: success     
 *********************************************************************************************/ 
char Get_ADC_Result(char *DataBuf) 
{      
  char *pDes = DataBuf;
  char *pSrc;
  
  if(Working_Buf) 
  {
    pSrc =  ADC_Cvt_Result_Buf0;
  } 
  else 
  {
    pSrc =  ADC_Cvt_Result_Buf1;
  }

  if(memcpy(pDes,pSrc, 2*MAX_ADC_CH))
    return 1;
  else
    return 0;
}

/**********************************************************************************************
 * Get_ADC_Ch_Result: This function is used to fetch the ADC conversion result of one channel
 *
 * Parameters:  Ch  : the channel number     
 *              Buf : point to the location which save the result
 *
 * Return:     0: failed, the conversion is not finished 
 *             1: success     
 *********************************************************************************************/ 
char Get_ADC_Ch_Result(char Ch, char *DataBuf) 
{
  char *pSrc;
  
  if(Ch >= MAX_ADC_CH)
    return 0;
  
  if(Working_Buf) 
  {
    pSrc =  ADC_Cvt_Result_Buf0;
  } 
  else 
  {
    pSrc =  ADC_Cvt_Result_Buf1;
  }

  *DataBuf = *(pSrc+ 2*Ch);
  *(DataBuf+1) = *(pSrc+ 2*Ch +1);

  return 1;
}

/****************************************************************************************
 * ADC_Start :  start to conversion of one channel
 * input:     None
 *
 * Return : None
 ***************************************************************************************/ 
char ADC_Start(void) 
{
  char Cvt_Ch = 0;
  unsigned long Compare_Val = 0;
  
  Current_Channel = 0;
  Time_Stamp._dword = 0;
  
  if( Total_Channels > 0) 
     Cvt_Ch = Work_Channel[0];
  else
     return 0;
  
  if(ADC_State == ADC_IDLE)
  {
     memset(ADC_Cvt_Result_Buf0, 0x00, 2* MAX_ADC_CH);
     memset(ADC_Cvt_Result_Buf1, 0x00, 2* MAX_ADC_CH);
     ADC_Results_Available = 0;
          
     if(Trig_Type == 0)             /*Software Trigger*/
     {
      ADC_State = ADC_BUSY;
      Config_Samp_Clk();
      ADCSC1 = Cvt_Ch;
     }
     else
      if(Trig_Type == 1)            /*Delay Trigger*/
      {
        //ADCSC2 = 0x40;
        //ADCSC1 =  Cvt_Ch; 
        //Set RTC value
        ADC_State = ADC_WAIT_TRIG;
        SET_RTC_TIME(Trig_Para1);
        SET_RTC_SEC();      
      } 
      else
       if(Trig_Type == 2)           /*Voltage level Trigger */
       {
         if(Trig_Para2)   
          ADCSC2 = 0x30;
         else
          ADCSC2 = 0x20;
          
          Compare_Val = Trig_Para3; 
          if(ADCCFG_MODE == 0b01) 
          {
            Compare_Val *= 4096;
            Compare_Val /= 100;
            ADCCVH = ((char)(Compare_Val>>8)) & 0x0F;
          }
          else
            if(ADCCFG_MODE == 0b10) 
            {
              Compare_Val *= 1024;
              Compare_Val /= 100;
              ADCCVH = ((char)(Compare_Val>>8)) & 0x03;
            }
            else 
            {
              Compare_Val *= 256;
              Compare_Val /= 100;
              ADCCVH = 0x00;
            }
          
            
          
          ADCCVL = (char)(Compare_Val);
          Cvt_Ch = Trig_Para1;
          
          ADC_State = ADC_WAIT_TRIG;
          ADCSC1 = (Cvt_Ch | 0x60);
       }
       
     return 1; 
    } 
    else 
    {
       if(ADC_State == ADC_WAIT_TRIG) 
         {
          ADCSC1 = Cvt_Ch;
          ADC_State = ADC_BUSY;
          return 1;
          } 
      else 
      {
        if(ADC_State == ADC_FINISH) 
         {
          ADC_State = ADC_BUSY;
          ADCSC1 = Cvt_Ch;
          return 1;
          } 
         else
          return 0;
      }
    }
 
}


/****************************************************************************************
 * ADC_Stop :  stop to conversion of one channel
 * input:     none
 *
 * Return : None
 ***************************************************************************************/ 
void ADC_Stop(void) 
{
   ADCSC1 = 0x00;
   ADCSC2 = 0x00;
   STOP_RTC();
   STOP_TPM();
   ADC_State = ADC_IDLE;
   
   return;
}

/****************************************************************************************
 * ADC_ISR: The ADC interrupt service routine which reads the ADC data register and places
 *         the value into global variables 
 *
 * Parameters: None
 *
 * Return : None
 ***************************************************************************************/ 
 interrupt void ADC_ISR (void)
 {
    dword *ptr;
    
    if(ADC_State == ADC_BUSY) 
    {
       if(Working_Buf) 
       {
          ADC_Cvt_Result_Buf1[2*Current_Channel] = ADCRH;
          ADC_Cvt_Result_Buf1[2*Current_Channel + 1] = ADCRL;
       } 
       else
       {
          ADC_Cvt_Result_Buf0[2*Current_Channel] = ADCRH;
          ADC_Cvt_Result_Buf0[2*Current_Channel + 1] = ADCRL;
       }
       
       
       if(Current_Channel < (Total_Channels -1)) 
       {
         Current_Channel ++;
       }
       else 
       {
         Time_Stamp._dword +=1;
         
         if(Working_Buf) 
         {
           ptr = &ADC_Cvt_Result_Buf1[2*Total_Channels];
         } 
         else 
         {
           ptr = &ADC_Cvt_Result_Buf0[2*Total_Channels];
         }
         
         *ptr =  Time_Stamp._dword;
         
         
         Current_Channel = 0;
         ADC_State = ADC_FINISH;
         Working_Buf ^= 0x01;
         ADC_Results_Available = 1;

         return;
       }
       
    }
    else 
    {
      if(ADC_State == ADC_WAIT_TRIG) 
      {
        ADCSC2 = 0x00;
        ADCSC2 = 0x00;
        Current_Channel = 0;
        ADC_State = ADC_BUSY;
        Config_Samp_Clk();
      }
    }
    
    ADCSC1 =  Work_Channel[Current_Channel]; 
    return;
 }


interrupt void  RTC_ISR(void) 
{
   RTCSC_RTIF = 1;
  
   if(ADC_State == ADC_WAIT_TRIG) 
   {
      //ADCSC2 = 0x00;
      STOP_RTC();
      //ADC_Start();
      ADC_State = ADC_BUSY;
      ADCSC1 = Work_Channel[Current_Channel];
      Config_Samp_Clk();
   } 
   else 
   {
       if(ADC_State == ADC_FINISH) 
        {
          ADC_State = ADC_BUSY;
          ADCSC1 = Work_Channel[Current_Channel]; 
          PTFD ^= 0x80;    //for test
        }
   }
   
   return;
}


interrupt void  TPM1_ISR(void) 
{
  if(TPM1SC_TOF)
    TPM1SC_TOF = 0;
  
  if(ADC_State == ADC_FINISH) 
  {
    ADC_State = ADC_BUSY;
    ADCSC1 = Work_Channel[Current_Channel];
    PTFD ^= 0x80;    //for test
  }
  return;
}