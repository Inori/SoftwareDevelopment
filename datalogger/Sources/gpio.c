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
 * File name   : GPIO.h
 * Description : This software defines the routines for GPIO
 *               
 *
 * History     :
 * 04/01/07  : Initial Development
 * 
 *************************************************************************************************/
#include <MC9S08JM60.h>
#include "gpio.h"

void Init_GPIO(void);
void Led_OnOff(char OnOff);
void Get_Button_Status(char *State);

/**********************************************************************************************
 * Init_GPIO: This function initializes the gpio module
 *
 * OverView:  All gpio pins are initialized to input
 * Parameters:      None
 *
 * Return:          none
 *********************************************************************************************/ 
void Init_GPIO(void) 
{
  PTAD = 0x00;
  PTADD = 0x20;         /*PTA5 output : accelerameter select 1*/
  PTADS = 0x00;
  PTAPE = 0x00;
  
  PTBD = 0x00;
  PTBDD = 0x00;
  PTBDS = 0x00;
  PTBPE = 0x00;
  
  PTCD = 0x14;
  PTCDD = 0xF4;//14;       /*PTC2,4; output, Led4, Led5*/
  PTCDS = 0x00;
  PTCPE = 0x00;
  
  PTDD = 0x04;
  PTDDD = 0x84;       /*PTD7,PTD2: output; select 2, Led 7 */
  PTDDS = 0x00;
  PTDPE = 0x00;
  
  PTED = 0x0C;
  PTEDD = 0x0C;       /*PTE2,3 : output, LED0, LED1 */
  PTEDS = 0x00;
  PTFPE = 0x00;
  
  PTFD = 0x23;
  PTFDD = 0xF3;//0x63;       /*PTF0,1,5,6: output LED2,3,6, accelerameter sleep*/
  PTFDS = 0x00;
  PTFPE = 0x00;
  
  
  PTGD = 0x00;
  PTGDD = 0x00;       /*PTG0-PTG3 : Buttons*/
  PTGDS = 0x00;
  PTGPE = 0x0F;
  
  return;
}

/**********************************************************************************************
 * Led_OnOff: This function is used to switch on or off the LEDs
 *
 * Parameters:      OnOff: From bit7(LED7) to Bit0(LED0): 
 *                              PTD2,PTF5,PTC4,PTC2,PTF1,PTF0,PTF0,PTE3,PTE2
 *
 * Return:          none
 *********************************************************************************************/ 
void Led_OnOff(char OnOff) 
{


  PTBD_PTBD0 = OnOff & 0x01 ?  0: 1;
  PTBD_PTBD1 = OnOff & 0x02 ?  0: 1;
/*
  PTCD_PTCD4 = (OnOff & 0x20)? 0: 1;
  PTCD_PTCD2 = (OnOff & 0x10)? 0: 1;
  
  PTDD_PTDD2 = (OnOff & 0x80)? 0: 1;
  
  PTED_PTED2  = (OnOff & 0x01)? 0: 1;
  PTED_PTED3  = (OnOff & 0x02)? 0: 1;

  PTFD_PTFD5 = (OnOff & 0x40)? 0 : 1;
  PTFD_PTFD0 = (OnOff & 0x04)? 0 : 1;
  PTFD_PTFD1 = (OnOff & 0x08)? 0 : 1;
  
*/  
  return;
}


/**********************************************************************************************
 * Led_OnOff: This function is used to switch on or off the LEDs
 *
 * Parameters:      OnOff: From bit7(LED7) to Bit0(LED0): 
 *                              PTD2,PTF5,PTC4,PTC2,PTF1,PTF0,PTF0,PTE3,PTE2
 *
 * Return:          none
 *********************************************************************************************/ 
void Get_Button_Status(char *State) 
{
  //*State = ((~ PTGD ) & 0x0F);
  *State = (PTBD  & 0x30)>>4;
  
  return;
}

