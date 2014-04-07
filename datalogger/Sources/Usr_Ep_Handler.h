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
 * File name   : Usr_Ep_Handler.h
 *
 * Description : This file deal the event on endpoint 1-6 
 *               
 *
 * History     :
 * 11/01/2007  : Initial Development
 * 
 *************************************************************************************************/
#ifndef _USR_EP_HANDLER
#define _USR_EP_HANDLER

#include "Usb_Bdt.h"

/* the endpoint for EP1, OUT direction*/
#define UEP1_CTL                EPCTL1
#define UEP1_BD                 Bdtmap.ep1Bio
#define UEP1_SIZE               16


/* the endpoint for EP2, OUT direction*/
#define UEP2_CTL                EPCTL2
#define UEP2_BD                 Bdtmap.ep2Bio
#define UEP2_SIZE               16


/* the endpoint for EP3, OUT direction*/
#define UEP3_CTL                EPCTL3
#define UEP3_BD                 Bdtmap.ep3Bio
#define UEP3_SIZE               32


/* the endpoint for EP5, OUT direction*/
#define UEP5_CTL                EPCTL5
#define UEP5E_BD                Bdtmap.ep5Bio_Even
#define UEP5O_BD                Bdtmap.ep5Bio_Odd
#define UEP5_SIZE               32

#define UEP5E_SIE_CTL           _SIE|_DATA0
#define UEP5O_SIE_CTL           _SIE|_DATA1

/* the other endpoint, begin*/

/* the other endpoint, end*/

extern char Usr_Ep_Buf_State; 

extern char Usr_Buf1[UEP1_SIZE];
extern char Usr_Buf2[UEP2_SIZE];

extern char Usr_Buf_Len[8];

extern pFunc Usr_Ep_Handler[6];

extern void Usr_Ep_Init(void);

extern char Get_Ep_State(char Ep, char Odd);
extern void Set_Ep_State(char Ep, char Odd);
extern void Clr_Ep_State(char Ep, char Odd);

extern char Is_Usr_Ep_Available(char Ep, char Odd); 
extern char Send_Data_Ep(char Ep, char *Data, char Size);
extern char Send_Data_PpEp(char Ep, char Odd, char *Data, char Size);


extern void Clear_Int(void);
extern void Recover_Int(void);

#endif