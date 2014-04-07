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
 * File name   : tpm.c
 * Description : This software defines the routines for TPM
 *
 * History     :
 * 04/01/07  : Initial Development
 * 
 *************************************************************************************************/
#include <MC9S08JM60.h>
#include "tpm.h"


#define TPM_INPUT_CLK   15000     //1.5MHz

void TPM_Init(void);
char TPM_Config(int Freq, char Duty);
void TPM_Stop(void);
void TPM_Start(void);


/**********************************************************************************************
 * TPM_Init: This function initilizes the TPM module
 *
 * Parameters:      none
 *
 *
 * Return:          void
 *********************************************************************************************/ 
 void TPM_Init(void) 
 {
    TPM1SC = 0x00;
    TPM2SC = 0x00;
    
    return;
 }
 
 
 /**********************************************************************************************
 * TPM_Init: This function initilizes the TPM module
 *
 * Parameters:      Freq:   (Hundred Hz) Range: 1-1000  
 *                  Duty:   0-100%
 *
 * Note:            Frequency = Freq * 100  (100Hz-100KHz)
 *
 * Return:          0: failed 1: success
 *********************************************************************************************/  
char TPM_Config(int Freq, char Duty) 
{
   unsigned int Mod = TPM_INPUT_CLK;
   unsigned long Cnt = 0x00;
   
   if(Freq > 1000)
    return 0;
   else
    Mod /= Freq;
   
   TPM2MOD = Mod;
   
   TPM2C0SC = 0x24;
   
   Cnt = (unsigned long)(Mod * Duty);
   Cnt /= 100;
   
   TPM2C0V = (int)Cnt;
   
   return 1;
}

 
/**********************************************************************************************
 * TPM_Init: This function initilizes the TPM module
 *
 * Parameters:      Num : 0 TPM1; 1: TPM2
 *
 *
 * Return:          void
 *********************************************************************************************/  
void TPM_Start(void) 
{
    TPM2CNT = 0;
    TPM2SC |= 0x0C;         //bus clock, divided by 8

  return;
}



/**********************************************************************************************
 * TPM_Init: This function initilizes the TPM module
 *
 * Parameters:      none
 *
 *
 * Return:          void
 *********************************************************************************************/ 
void TPM_Stop(void) 
{
   char Dummy;
   
   Dummy = TPM2SC;
   TPM2SC &= 0x67;         //bus clock, divided by 8
  
  return;
}
 