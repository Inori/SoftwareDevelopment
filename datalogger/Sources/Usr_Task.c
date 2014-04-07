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
 * File name   : Command_Parse.c
 *
 * Description : This file deal all events on endpoint 0 in enumeration process
 *               
 *
 * History     :
 * 04/01/2007  : Initial Development
 * 
 *************************************************************************************************/
#include <MC9S08JM60.h>
#include "Usr_Task.h"
#include "Usr_Ep_handler.h"
#include "gpio.h"
#include "adc.h"
#include "tpm.h"


char Cmd_Exe_Flag = 0;                /*0: Not executed, 1: Failed 2: success*/
char Data_Logging = 0;

void Usr_Task(void);

/**********************************************************************************************
 * Exe_Command: This routine checks the command received from host, then parse the command packet
 *              execute the command, provide the response to host
 *
 * Parameters:      None
 *
 * Return:          none
 *********************************************************************************************/ 
void Usr_Task(void) 
{
  
  if(Data_Logging) 
  {
    if(ADC_Results_Available) 
    {
       if(Working_Buf)
       {
        if(Is_Usr_Ep_Available(5,0)) 
        {
          ADC_Results_Available = 0;
          Send_Data_PpEp(0x05,0,ADC_Cvt_Result_Buf0, Valid_Data_Len);
        }
       } 
       else 
       {
        if(Is_Usr_Ep_Available(5,1)) 
        {
          ADC_Results_Available = 0;
          Send_Data_PpEp(0x05,1,ADC_Cvt_Result_Buf1, Valid_Data_Len);
        }
       }
         
    }
  }
    
  if(Get_Ep_State(1,0)) 
  {
    Usr_Buf2[0] = Usr_Buf1[0];
    Usr_Buf2[1] = Usr_Buf1[1];
    
    switch(Usr_Buf1[0]) 
    {
      case 0:                           /*MCU*/
        if(Usr_Buf1[1] == 0x01) 
        {
          while(1);                     /*Reset MCU by watch-dog*/
        } 
        else 
        {
           Usr_Buf2[2] = 0x00;
           Usr_Buf_Len[1] = 3;
        }
      break;
      
      case 1:                           /*Boot Loader*/
        if(Usr_Buf1[1] == 0x01) 
        {
           while(1);                      /*Reset MCU by watch-dog*/
          
        }
        else 
        {
           Usr_Buf2[2] = 0x00;
           Usr_Buf_Len[1] = 3;
        } 
      break;
      
      case 2:                           /*ADC*/
        switch(Usr_Buf1[1]) 
        {
          case 0x00:                    /*request ADC conversion result*/
            Usr_Buf2[2] = Usr_Buf1[2];
            if(Get_ADC_Ch_Result(Usr_Buf2[2], &Usr_Buf2[3])) 
            {
              Usr_Buf_Len[1] = 0x05;
            } 
              
          break;
          
          case 0x01:                    /*Configure ADC*/
            if(ADC_Config(Usr_Buf1[2], Usr_Buf1[3],Usr_Buf1[4],Usr_Buf1[5])) 
            {
              Usr_Buf2[2] = 0xFF;
            } 
            else 
            {
              Usr_Buf2[2] = 0x00;
            }
            
            Set_Trigger_Type(Usr_Buf1[6], Usr_Buf1[7], Usr_Buf1[8], Usr_Buf1[9]);
            
            Usr_Buf_Len[1] = 0x03;
           
          break;
          
          case 0x02:                    /*Return ADC Configuration*/
            Get_ADC_Configuration(&Usr_Buf2[2]);
            Usr_Buf_Len[1] = 10;
            
          break;
          
          case 0x03:                    /*Start ADC*/
            if(ADC_Start())
            {
              Usr_Buf2[2] = 0xFF;
              Data_Logging = 1;
            } 
            else 
            {
              Usr_Buf2[2] = 0x00;
            }
            
            Usr_Buf_Len[1] = 0x03;
            
          break;
          
          case 0x04:                    /*Stop ADC conversion*/
           ADC_Stop();
           Data_Logging = 0;
           
           Usr_Buf2[2] = 0xFF;
           Usr_Buf_Len[1] = 0x03;
          break;
          
          case 0x05:                    /*return ADC state*/
            Usr_Buf2[2] = Get_Adc_State();
            Usr_Buf_Len[1] = 0x03;
          break;
          
          default:
            Usr_Buf2[2] = 0x00;
            Usr_Buf_Len[1] = 0x03;
          break;
        }
      break;
      
      case 6:                           /*GPIO*/
         if(Usr_Buf1[1] == 0x00) 
         {
           Led_OnOff(Usr_Buf1[2]);
           Usr_Buf2[2] = 0xFF;
           Usr_Buf_Len[1] = 3;
         } 
         else
         if(Usr_Buf1[1] == 0x01) 
         {
            Get_Button_Status(&Usr_Buf2[2]);
            Usr_Buf_Len[1] = 3;
         }
      break;
      
      case 7:                           /*TPM*/
         switch(Usr_Buf1[1]) 
         {
          case 0:                       /*Start PWM*/
             if(TPM_Config((int)((Usr_Buf1[2]<<8)| Usr_Buf1[3]), Usr_Buf1[4])  )
             {
               TPM_Start();
               Usr_Buf2[2] = 0xFF;
               Usr_Buf_Len[1] = 3;
             } 
             else
             {
               Usr_Buf2[2] = 0x00;
               Usr_Buf_Len[1] = 3;
             }
             
             
          break;
          
          case 1:                       /*Stop PWM*/
             TPM_Stop();
             Usr_Buf2[2] = 0xFF;
             Usr_Buf_Len[1] = 3;
          break;
          
          default:
             Usr_Buf2[2] = 0x00;
             Usr_Buf_Len[1] = 3;
          break;
         }
         
      break;
    }

    Cmd_Exe_Flag = 1;
    Clr_Ep_State(1,0);    /*clear the bit, since this command has been processed*/
  } 
 
     /*Transfer the data to host*/
  if(Cmd_Exe_Flag) 
   {
     if(Is_Usr_Ep_Available(2,0)) 
     {
       Send_Data_Ep(2,Usr_Buf2,Usr_Buf_Len[1]);
       Set_Ep_State(2,0);
       Cmd_Exe_Flag = 0;
     } 
   }
  
  return;
}
