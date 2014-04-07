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
 * File name   : Usb_Setup_Pkt.c
 *
 * Description : This file deal all events on endpoint 0 in enumeration process
 *               
 *
 * History     :
 * 04/01/2007  : Initial Development
 * 
 *************************************************************************************************/
#include <MC9S08JM60.h>
#include <stddef.h>
#include <string.h>
#include "Usr_Ep_Handler.h"
#include "Usb_Bdt.h"
#include "Usb_Drv.h"
#include "typedef.h"

/*Global Var definition*/
char Usr_Buf1[UEP1_SIZE];
char Usr_Buf2[UEP2_SIZE];

char Usr_Buf_Len[8];     /*the atual data length which will be transffered or has been received*/



/*Local Var definition*/
char Working_Int = 0x00; /*Save the interrupt flag before USB enter into suspend (STOP3 mode)*/
char Usr_Ep_Buf_State;   /*OUT: 0: no data received, 1: data received*/
                         /*IN:  0: no data delivered,1: data is delivering, buffer is controoled by SIE*/
                         /*bit0: Ep1, bit2: Ep1,... Bit4: Ep5 Even,Bit5: Ep5 ODD,... bit7: Ep5 Odd....*/



/* the definition for usr endpoint buffer, it should be started from 0x18A0*/
char UEp1_Buffer[UEP1_SIZE]    @0x18A0;
char UEp2_Buffer[UEP2_SIZE]    @0x18B0;
char UEp3_Buffer[UEP3_SIZE]    @0x18C0;
char UEp5E_Buffer[UEP5_SIZE]    @0x18E0;
char UEp5O_Buffer[UEP5_SIZE]    @0x1900;




void Usr_Ep_Init(void);

void Ep1_Handler(void);
void Ep2_Handler(void);
void Ep3_Handler(void);
void Ep5_Handler(void);

pFunc Usr_Ep_Handler[6] = { &Ep1_Handler, &Ep2_Handler, &Ep3_Handler, NULL, &Ep5_Handler, NULL };


char Get_Ep_State(char Ep, char Odd);
void Set_Ep_State(char Ep, char Odd);
void Clr_Ep_State(char Ep, char Odd);
char Is_Usr_Ep_Available(char Ep, char Odd);              /*Is the EP controlled by CPU?*/       
char Send_Data_Ep(char Ep, char *Data, char Size);
char Send_Data_PpEp(char Ep, char Odd, char *Data, char Size); 


void Clear_Int(void);
void Recover_Int(void);

/**********************************************************************************************
 * Usr_Ep_Init: This function initilizes the Usr Eps
 *
 * Parameters:      none
 *
 * Return:          void
 *********************************************************************************************/ 
void Usr_Ep_Init(void) 
{
    /*Endpoint 1 initialization*/
    UEP1_CTL = EP_OUT|HSHK_EN;                
    
    UEP1_BD.Cnt = UEP1_SIZE;    
    UEP1_BD.Addr =  0x10;     //( (byte)( (&UEp1_Buffer) - 0x1860)  )>>2 ; //can be calculated out
    UEP1_BD.Stat._byte = _SIE|_DATA0|_DTS; 

    /*Endpoint 2 initialization*/
    UEP2_CTL = EP_IN|HSHK_EN;                
    UEP2_BD.Addr = 0x14;       //( (byte)( (&UEp2_Buffer) - 0x1860) >> 2 );      // Set buffer address
    UEP2_BD.Stat._byte = _CPU|_DATA1;       
    
    /*Endpoint 3 initialization*/
    UEP3_CTL = EP_OUT|HSHK_EN;                
    
    UEP3_BD.Cnt = UEP3_SIZE;    
    UEP3_BD.Addr =  0x18;     //( (byte)( (&UEp3_Buffer) - 0x1860)  )>>2 ; //can be calculated out
    UEP3_BD.Stat._byte = _SIE|_DATA0|_DTS; 

    /*Endpoint 5 initialization*/
    UEP5_CTL = EP_IN|HSHK_EN;                
    UEP5E_BD.Addr = 0x20;       //( (byte)( (&UEp4_Buffer) - 0x1860) >> 2 );      // Set buffer address
    UEP5E_BD.Stat._byte = _CPU|_DATA0;  
    
    UEP5O_BD.Addr = 0x28;       //( (byte)( (&UEp4_Buffer) - 0x1860) >> 2 );      // Set buffer address
    UEP5O_BD.Stat._byte = _CPU|_DATA1;       
     
    
    /*Other EPs initialization, begin*/
    
    /*Other EPs initialization, end*/
    
    Usr_Ep_Buf_State = 0;
    Usr_Buf_Len[0] = 0;
    Usr_Buf_Len[1] = 0;
    Usr_Buf_Len[2] = 0;
    Usr_Buf_Len[4] = 0;
    
    return;
}



/**********************************************************************************************
 * EP1_Handler: This function handles the event on ep1
 *
 * Parameters:      none
 *
 * Return:          void
 *********************************************************************************************/ 
void Ep1_Handler(void) 
{
  char i;
  
  Usr_Buf_Len[0] = UEP1_BD.Cnt;
  for (i=0;  i < Usr_Buf_Len[0] ; i++)
      Usr_Buf1[i] = UEp1_Buffer[i];
      
  Usr_Ep_Buf_State |=  0x01;    
  
  UEP1_BD.Cnt = UEP1_SIZE; 
  USB_Buf_Rdy(&UEP1_BD);
  return;
}


/**********************************************************************************************
 * EP2_Handler: This function handles the event on ep2
 *
 * Parameters:      none
 *
 * Return:          void
 *********************************************************************************************/ 
void Ep2_Handler(void) 
{
   Usr_Ep_Buf_State &=  0xFD;     /* clear bit1, means Ep2 is available*/ 
   return;
}


/**********************************************************************************************
 * EP3_Handler: This function handles the event on endpoint 3, OUT direction
 *
 * Parameters:      none
 *
 * Return:          void
 *********************************************************************************************/ 
void Ep3_Handler(void) 
{
  Usr_Ep_Buf_State |=  0x04;
  UEP3_BD.Cnt = UEP1_SIZE; 
  USB_Buf_Rdy(&UEP3_BD);
  return;
}


/**********************************************************************************************
 * EP5_Handler: This function handles the event on ep5, IN direction
 *
 * Parameters:      none
 *
 * Return:          void
 *********************************************************************************************/ 
void Ep5_Handler(void) 
{
   if(STAT_ODD)
     Usr_Ep_Buf_State &=  0xDF;     /* clear bit5, means Ep5 ODD is available for CPU*/ 
   else
     Usr_Ep_Buf_State &=  0xE7;     /* clear bit4, means Ep5 EVEN is available for CPU*/ 
   return;
}


/**********************************************************************************************
 * EP2_Handler: This test if the endpoint state is logic 1
 *
 * Parameters:      Ep: endpoint number  0 < Ep < 8
 *                  Odd: 0: even buffer 1: Odd buffer; This bit is always 0 for EP 1-4
 *
 * Return:          OUT EP: 0:No data received, 1: Data received
 *                  IN  EP: 0:EP is free,       1: EP is controlled by SIE
 *********************************************************************************************/ 
char Get_Ep_State(char Ep, char Odd) 
{
   if(Usr_Ep_Buf_State & ( 1 << ((Ep - 1) + Odd )))
    return 1;
   else
    return 0;
}

/**********************************************************************************************
 * Set_Ep_State: This routine set the ep state bit
 *
 * Parameters:      Ep: endpoint number  0 < Ep < 8
 *                  Odd: 0: even buffer 1: Odd buffer; This bit is always 0 for EP 1-4
 *                  Odd: 0: even buffer 1: Odd buffer; This bit is always 0 for EP 1-4
 *
 * Return:          OUT EP: 0:No data received, 1: Data received
 *                  IN  EP: 0:EP is free,       1: EP is controlled by SIE
 *********************************************************************************************/ 
void Set_Ep_State(char Ep, char Odd) 
{
  Usr_Ep_Buf_State |= (1<<((Ep-1) + Odd));
  return;
}

/**********************************************************************************************
 * EP2_Handler: This routine clear the  state ep bit
 *
 * Parameters:      Ep: endpoint number  0 < Ep < 8
 *                  Odd: 0: even buffer 1: Odd buffer; This bit is always 0 for EP 1-4
 *
 * Return:          OUT EP: 0:No data received, 1: Data received
 *                  IN  EP: 0:EP is free,       1: EP is controlled by SIE
 *********************************************************************************************/ 
void Clr_Ep_State(char Ep, char Odd)
{
   Usr_Ep_Buf_State &= (~((1<<((Ep-1)+Odd))));
   return;
}

/**********************************************************************************************
 * Is_Ep_Available: This routine check if the CPU controls the BDT and buffer of one EP
 *
 * Parameters:      Ep: endpoint number  1 < Ep < 8,
 *                  Odd: for Ep5,6, we need to check that it is even buffer or odd buffer
 *
 * Return:          1: controlled by CPU
 *                  0: contrilled by SIE
 *********************************************************************************************/ 
char Is_Usr_Ep_Available(char Ep, char Odd)
{
   BD_STAT Tmp;
   
   switch(Ep) 
   {
    case 1:
      Tmp = UEP1_BD.Stat;
    break;
    
    case 2:
      Tmp = UEP2_BD.Stat;
    break;         
    
    case 3: 
      Tmp = UEP3_BD.Stat;
    break;
    
    case 4:
      Tmp = Bdtmap.ep4Bio.Stat;
    break;
    
    case 5:
      if(Odd)
        Tmp = Bdtmap.ep5Bio_Odd.Stat;
      else
        Tmp = Bdtmap.ep5Bio_Even.Stat;
    break;
    
    case 6:
      if(Odd)
        Tmp = Bdtmap.ep6Bio_Odd.Stat;
      else
        Tmp = Bdtmap.ep6Bio_Even.Stat;
    break;
    
    default:
    return 0;
   }
   
   if(Tmp.McuCtlBit.OWN)
      return 0;
   else
      return 1;
}

/**********************************************************************************************
 * Send_Data_Ep: This function write the data to IN endpoint buffer and make the endpoint ready
 *               to send data
 *
 * Parameters:   Ep : Endpoint
 *               Data: point to the data buffer which will be sent
 *               Size: the data length which will be delivered   
 *
 * Return:        0: failed ; 1: success
 *********************************************************************************************/ 
char Send_Data_Ep(char Ep, char *Data, char Size)
{
    BUFF_DSC  *pBdt;
    char      *pUsrBuf = Data;
    char      *pEpBuf;
   
    switch(Ep) 
    {
      case 1:         /*Ep1 is OUT direction*/
       return 0;
       
      case 2:
        pBdt=&UEP2_BD;
        pEpBuf=UEp2_Buffer;
        break; 
        
      case 3:
      case 4:
        return 0;

      default:        
      break;
    }
      
    /* Copy buffer to USB RAM */
    memcpy(pEpBuf,pUsrBuf,Size);
    
    /* Set Packet Size and activate EP */
    pBdt->Cnt= Size;
    USB_Buf_Rdy(pBdt);
      
    return 1;
}

/**********************************************************************************************
 * Send_Data_PpEp: This function write the data to Ping-pong IN endpoint buffer and make the endpoint ready
 *               to send data
 *
 * Parameters:   Ep : Endpoint (5 or 6)
 *               Data: point to the data buffer which will be sent
 *               Size: the data length which will be delivered   
 *
 * Return:        0: failed ; 1: success
 *********************************************************************************************/ 
char Send_Data_PpEp(char Ep, char Odd, char *Data, char Size) 
{
  BUFF_DSC  *pBdt;
  char      *pUsrBuf = Data;
  char      *pEpBuf;
  
  if(Ep == 0x05) 
  {
     if(Odd) 
     {
       pBdt=&UEP5O_BD;
       pEpBuf=UEp5O_Buffer;
       memcpy(pEpBuf,pUsrBuf,Size);
       pBdt->Cnt= Size;
       pBdt->Stat._byte = UEP5O_SIE_CTL;
     } 
     else 
     {
       pBdt=&UEP5E_BD;
       pEpBuf=UEp5E_Buffer;
       memcpy(pEpBuf,pUsrBuf,Size);
       pBdt->Cnt= Size;
       pBdt->Stat._byte = UEP5E_SIE_CTL;
     }
     
     return 1;
  } 
  else 
  {
     return 0;
  }
}

/**********************************************************************************************
 * Clear_Int: This routine is used to clear unecessary interrupt which should be disable when 
 *            USB enter into suspend state
 *
 * Parameters:   None
 *
 * Return:       None
 *********************************************************************************************/ 
void Clear_Int(void) 
{
  if(RTCSC_RTIE) 
  {
    RTCSC_RTIE = 0 ;
    Working_Int |= 0x01;
  }
  
  if(RTCSC_RTIE) 
  {
    TPM1SC_TOIE = 0 ;
    Working_Int |= 0x02;
  }

  return;
}


/**********************************************************************************************
 * Recover_Int: This routine is used to recover the interrupt which should be disable when 
 *            USB enter into suspend state
 *
 * Parameters:   None
 *
 * Return:       None
 *********************************************************************************************/ 
void Recover_Int(void) 
{
  if(Working_Int & 0x01)
    RTCSC_RTIE = 1;
  
  if(Working_Int & 0x02)
    TPM1SC_TOIE = 1;
  return;
}


