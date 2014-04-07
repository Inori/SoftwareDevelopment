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
 /************************************************************************
 * File name   : Usb_Bdt.c
 * Description : This file provides the USB driver, all USB events will be 
 *               precessed
 *               
 *
 * History     :
 * 04/01/2007  : Initial Development
 * 
 *************************************************************************/
#include <MC9S08JM60.h>
#include <stdlib.h>
#include "typedef.h"
#include "Usb_Config.h"
#include "Usb_Drv.h"
#include "Usb_Bdt.h"
#include "Usb_Ep0_Handler.h"
#include "Usr_Ep_Handler.h"


/*Global variable definition*/

byte Usb_Device_State;          /* device states*/
USB_DEVICE_STATUS Usb_Stat;     /* global flags*/
byte Usb_Active_Cfg;            /* value of current configuration*/
byte Usb_Alt_Intf[MAX_NUM_INTF]; /* array to keep track of the current alternate*/
                                /* setting for each interface ID*/

/* buffer descriptor table entry*/
/* It locates the beginning of USb RAM,   0x1860 - 0x195F (256 bytes)*/
BDTMAP Bdtmap @0x1860;

/*endpoint 0 buffer definition*/
CTRL_TRANSFER_DATA  CtrlTrf_Data @0x1880;
CTRL_TRANSFER_SETUP Setup_Pkt    @0x1890;

void Check_USB_Status(void);

void USB_Suspend(void);
void USB_WakeFrom_Suspend(void);
unsigned char USB_WakeUp(void );

void USB_Bus_Reset_Handler(void);
void USB_SOF_Handler(void);
void USB_Stall_Handler(void);
void USB_Error_Handler(void);

void USB_Transaction_Handler(void);

void interrupt  USB_ISR();
void interrupt  IRQ_ISR();



/******************************************************************************
 * Function:        void Initialize_USBModule(void)
 * Input:           None
 * Output:          None
 * Overview:        initialize the USB module
 *                  enable regulator, PHY, pull-up resistor, BDT initialization
 *
 * Note:            None
 *****************************************************************************/
void  Initialize_USBModule()
{
  char i;
  unsigned char *pUsbMem = (unsigned char *) &Bdtmap;

  
  Usb_Device_State = POWERED_STATE;
  #ifdef SELF_POWER
    Usb_Stat._byte = 0x01;                /*SelfPower*/
  #else
    Usb_Stat._byte = 0x00;                /*Notice: Bus powered*/
  #endif
  Usb_Active_Cfg = 0x00;

  USBCTL0 = USB_RST;
  while(USBCTL0_USBRESET)
  {
    
  };
  
  
  for(i= 0; i < 0xFF; i++)
  {
    *pUsbMem = 0x00;
    pUsbMem ++; 
  }   /*the USB RAM initialization is just for test,it can be deleted*/
  
  
  /*Below is initialize the BDT*/
  Bdtmap.ep0Bo.Stat._byte = _SIE|_DATA0|_DTS;  /* endpoint 0 OUT initialization*/
  Bdtmap.ep0Bo.Cnt = EP0_BUFF_SIZE;              
  Bdtmap.ep0Bo.Addr = 0x0C;                       //((byte)(((byte*) &Setup_Pkt)-0x1860)  )>> 2;  //can calculate this value  

  Bdtmap.ep0Bi.Stat._byte = _CPU;                /* endpoint 0 IN buffer initialization*/
  Bdtmap.ep0Bi.Cnt = 0x00;                      
  Bdtmap.ep0Bi.Addr = 0x08;                     

  USBCTL0 = 0X00;
  
  EPCTL0 = 0x0D;    /*enable endpoint 0*/
  INTENB = 0x01;
}


/******************************************************************************
 * Function:        void Check_USB_Status(void)
 * Input:           None
 * Output:          None
 * Overview:        This function can be used to detect if the USB bus has attached
 *                  on USB bus, we can use a GPIO, or KBI interrupt, it is disable here
 *
 *****************************************************************************/
void Check_USB_Status(void)
{
    #ifdef SELF_POWER
    if(PTGD_PTGD0 == USB_BUS_ATTACHED)         /* Is JM60 attached on USB bus? */
     {  
    #endif
        if(CTL_USBEN == 0)                     /* JM60 module off ? */
         {
            EPCTL0 = 0x0D;
            INTSTAT = 0xBF;
            CTL = 0x00;
            INTENB = 0x00;                      /* disable USB interrupt*/
            CTL_USBEN = 0x01;                   /* enable module */
            USBCTL0 = UCFG_VAL;                 /* attach JM60 to USB bus*/

            Usb_Device_State = ATTACHED_STATE;      
         }
  
   #ifdef SELF_POEWER
     }
    else
    {
        if(CTL_USBEN == 1)                      /*  JM60 USB on ?*/
         {
             CTL = 0x00;                        /* disable module */
             INTENB = 0x00;                     /* mask USB interrupt*/
             Usb_Device_State = POWERED_STATE;      
          }
    }
   #endif
   
     if(Usb_Device_State == ATTACHED_STATE)
      {
         INTSTAT = 0xBF;                        /*clear USB interrupts*/
         INTENB = 0xBF;                            
      }


    if(Usb_Device_State == USB_ENTER_SUSPEND)
    {
        USB_Suspend();
    }
}



/******************************************************************************
 * Function:        void USB_Soft_Detach(void)
 * Input:           None
 * Output:          None
 * Overview:        USB_Soft_Detach disconnects the device from USB bus. 
 *                  This is done by stop supplying Vusb voltage to
 *                  pull-up resistor. The pull-down resistors on the host
 *                  side will pull both differential signal lines low and
 *                  the host registers the event as a disconnect.
 *
 *                  Since the USB cable is not physically disconnected, the
 *                  power supply through the cable can still be sensed by
 *                  the device. The next time Check_USBBus_Status() function
 *                  is called, it will reconnect the device back to the bus.
 *****************************************************************************/
void USB_Soft_Detach(void)
{
   CTL = 0x00;                             /* disable module */
   INTENB = 0x00;                          /* mask USB interrupt*/
   Usb_Device_State = POWERED_STATE;      
   return;
}





/******************************************************************************
 * Function:        void USB_Suspend(void)
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    None
 * Overview:        After 3ms of USB bus idle status, the SLEEPF will be set,
 *                  The firmware should enable the resume function in 7ms before 
 *                  the USB enters into suspend state.       
 * Note:            None
 *****************************************************************************/
void USB_Suspend(void)
{
    unsigned char Result;
   
    //INTENB_RESUME = 1;   
    USBCTL0_USBRESMEN = 1;
                     
    INTSTAT_SLEEPF = 1;   
    #ifndef SELF_POWERED                 
      Clear_Int();          /*disable the int which will wake the MCU from stop3*/
    #endif

   
    Usb_Device_State = USB_SUSPEND;
    do
      {
        Result = USB_WakeUp() ;
      }while(!Result);
      
    USBCTL0_USBRESMEN = 0;  
    
    if(Result == 2)
        USB_Remote_Wakeup();
    
    #ifndef SELF_POWERED
      Recover_Int();    /*recover the diabled int*/
    #endif                  
  	return;
}

/******************************************************************************
 * Function:        void USB_WakeFrom_Suspend(void)
 * Input:           None
 * Output:          None
 * Overview:        
 *****************************************************************************/
void USB_WakeFrom_Suspend(void)
{
    INTSTAT_RESUMEF = 1;  /*write 1 to clear*/
    INTENB_RESUME = 0;
    CTL_TSUSPEND = 0;	    /*enable the transmit or receive of packet */

}
/******************************************************************************
 * Function:        void USB_Remote_Wakeup(void)
 * Input:           None
 * Output:          None
 * Overview:        This function is used to send wake-up signal from device to
 *                  host or hub
 *
 * Note:           According to USB 2.0 specification section 7.1.7.7,
 *                  "The remote wakeup device must hold the resume signaling
 *                  for at lest 1 ms but for no more than 15 ms."
 *****************************************************************************/
void USB_Remote_Wakeup(void)
{
    static word delay_count;

    USB_WakeFrom_Suspend();   
     
    CTL_CRESUME = 1;          /* Start RESUME signaling*/
       
    delay_count = 5000;       /* Set RESUME line for 1-15 ms*/
     do
     {
       delay_count--;
       __RESET_WATCHDOG();
     }while(delay_count);        
        

    CTL_CRESUME = 0;
}

/******************************************************************************
 * Function:        void USB_SOF_Handler(void)
 * Input:           None
 *
 * Output:          None
 * Overview:        The USB host sends out a SOF packet to full-speed devices
 *                  every 1 ms. This interrupt may be useful for isochronous
 *                  pipes. 
 *****************************************************************************/
void USB_SOF_Handler(void)
{
    /*can add some code if isochronous transfer*/
    
    INTSTAT_SOFTOKF = 1;   /*clear the interrupt flag*/
}

/******************************************************************************
 * Function:        void USB_Stall_Handler(void)
 * Input:           None
 * Output:          None
 * Overview:        The STALLIF is set anytime the SIE sends out a STALL
 *                  packet regardless of which endpoint causes it.
 *                  A Setup transaction overrides the STALL function. A stalled
 *                  endpoint stops stalling once it receives a setup packet.
 *
 *                  There are a few reasons for an endpoint to be stalled.
 *                  1. When a non-supported USB request is received.
 *                     Example: GET_DESCRIPTOR(DEVICE_QUALIFIER)
 *                  2. When an endpoint is currently halted.
 *                  3. When the device class specifies that an endpoint must
 *                     stall in response to a specific event.
 *
 *****************************************************************************/
void USB_Stall_Handler(void)
{
    if((STAT & 0xF0)== 0x00)
    {
      if(EPCTL0_EPSTALL == 1)
        EPCTL0_EPSTALL = 0;
          
      /*Notice if is generated by BDTSTALL, the firmware should notice the IN or OUT DIR*/
      /*we only set it for OUT direction in this demo*/
      USB_Prepare_Next_Trf();     
    }
    
    /*
    else
     {
      //Add the process for STALL generated by other endpoints (even or odd)
     }
    */
    
    INTSTAT_STALLF = 1;
}



/******************************************************************************
 * Function:        void USB_Error_Handler(void)
 * Input:           None
 * Output:          None
 * Overview:        You can check error register to see which error occured
 * Note:            None
 *****************************************************************************/
void USB_Error_Handler(void)
{
    INTSTAT_ERRORF = 1;
    /*add code to check the error source */

    if((ERRSTAT_BUFERRF) && ((STAT & 0xF0)==0)) /*Bdt buffer overflow*/
      {
        USB_Prepare_Next_Trf();  
      }
      
    /*check the other error at here */
      
    ERRSTAT = 0xBF; /*clear all errors*/
    return;
}



/******************************************************************************
 * Function:        void USB_Bus_Reset_Handler(void)
 *
 * Input:           None
 * Output:          None
 *
 * Overview:        Once a USB bus reset is received from the host, this
 *                  function should be called. It resets the device address to
 *                  zero, disables all non-EP0 endpoints, initializes EP0 to
 *                  be ready for default communication, clears all USB
 *                  interrupt flags, unmasks applicable USB interrupts, and
 *                  reinitializes internal state-machine.
 *****************************************************************************/
void USB_Bus_Reset_Handler(void)
{
    ERRSTAT = 0xFF;                 /* clear USB error flag*/
    INTSTAT = 0xBF;                 /* clear USB interrupt*/
    ERRENB = 0xBF;                  /* disable all USB error interrupt sources*/
   	INTENB = 0x9B;//F;                  /* enable all interrupts except RESUME*/
	
    ADDR = 0x00;                    /* reset to default address*/
    Clear_Mem((byte*)&EPCTL1,6);    /* disable all endpoints */
    EPCTL0 = EP_CTRL|HSHK_EN;       /* enable endpoint 0*/

    while(INTSTAT_TOKDNEF )         /* Flush any pending transactions because 4 status buffer*/
        INTSTAT = 0xBF;              

    USB_Prepare_Next_Trf();   /*prepare to receive the setup packet*/

  	
    Usb_Stat.BitCtl.RemoteWakeup = 0;       /* default status flag to disable*/
    Usb_Active_Cfg = 0;                    
    Usb_Device_State = DEFAULT_STATE;
    
}


/******************************************************************************
 * Function:        void USB_Buf_Rdy(buffer_dsc)
 *
 * Input:           byte buffer_dsc: one buffer descriptor in Bdtmap
 * Output:          None
 *
 * Overview:        This function turns the buffer ownership to SIE,
 *                  and toggles the DTS bit for synchronization.
 * Note:            This function should not be called by Endpoint 5 or 6
 *                  because they are pingpong buffer, DATA0/1 do not need 
 *                  to be switched
 *****************************************************************************/
void USB_Buf_Rdy(BUFF_DSC *buffer_dsc)
{                                                        
    buffer_dsc->Stat._byte &= _DATA1;          
    buffer_dsc->Stat.McuCtlBit.DATA = ~ buffer_dsc->Stat.McuCtlBit.DATA; /* Toggle DTS bit*/
    buffer_dsc->Stat._byte |= _SIE|_DTS;                              /*give the ownership to SIE*/
}



/******************************************************************************
 * Function:        void Clear_Mem(byte* startAdr,byte count)
 * Input:          
 * Output:          None
 * Overview:        None
 *****************************************************************************/
void Clear_Mem(byte* startAdr,byte count)
{
    byte i;
        
    for(i=0; i < count; i++)
    {
       *(startAdr + i) = 0x00;
    }
    
    return;
}



/******************************************************************************
 * Function:       void interrupt  USB_ISR()
 * Input:          
 * Output:          None
 * Overview:        The USB stat interrupt service routine
 * Note:            None
 *****************************************************************************/
void interrupt  USB_ISR()
{
    
  if((USBCTL0_LPRESF) && (Usb_Device_State == USB_SUSPEND))
    {
      USBCTL0_USBRESMEN = 0;
    }
  
  
  if(INTSTAT_RESUMEF && INTENB_RESUME)
     USB_WakeFrom_Suspend();

     	
  if(INTSTAT_USBRSTF && INTENB_USBRST) 
    {
      USB_Bus_Reset_Handler();
    }
   

  if(INTSTAT_SOFTOKF && INTENB_SOFTOK)
    {
      USB_SOF_Handler();
    }
    
  if(INTSTAT_STALLF && INTENB_STALL )
      USB_Stall_Handler();
    
  if(INTSTAT_ERRORF && INTENB_ERROR)
      USB_Error_Handler(); 
   
     
  if((INTSTAT_SLEEPF && INTENB_SLEEP)&&(Usb_Device_State >= CONFIGURED_STATE))  
    {
      Usb_Device_State = USB_ENTER_SUSPEND; /*do not hope enter into suspend */
      INTSTAT_SLEEPF = 1; 
    }
                
  if(INTSTAT_TOKDNEF && INTENB_TOKDNE)
    {
      USB_Transaction_Handler();
        
      INTSTAT_TOKDNEF = 1;     /*write 1 to clear*/
    }
    
   return;
}


 
/******************************************************************************
 * Function:        void USB_CtrlEP_Handler(void)
 * Input:           None
 * Output:          None
 * Overview:        process the transaction from different endpoint
 *****************************************************************************/
void USB_Transaction_Handler(void)
{   
   unsigned char stat = STAT;
    
   if((stat & 0xF0 )  == 0x00)
   {
     stat &= 0xF8;  
    
     if(stat == EP00_OUT)
      {
        if(Bdtmap.ep0Bo.Stat.RecPid.PID == SETUP_TOKEN)    
            USB_CtrlTrf_SetupStage_Processing();
        else                                              
            USB_CtrlTrf_OUT_Handler();
      }
     else 
      {
        if(stat == EP00_IN)                                 
          USB_CtrlTrf_IN_Handler();
      }
   }
   
   else
    {
      stat= ((stat >> 4) - 1);
      
      if(Usr_Ep_Handler[stat] != NULL)  /* if the Ep handler is not NULL, then call*/
        Usr_Ep_Handler[stat] ();    
     
    }
    
   return;
}


/******************************************************************************
 * Function:       void interrupt  IRQ_ISR()
 * Input:          
 * Output:          None
 * Overview:        The IRQ is used to wakup the USB and MCU.
 * Note:            None
 *****************************************************************************/
void interrupt IRQ_ISR()
{
   IRQSC_IRQACK = 1;
   return;
}

/******************************************************************************
 * Function:        unsigned char USB_WakeUp()
 * Input:          
 * Output:          0: noise, re-entern into sleep
 *                  1: Bus wake-up
 *                  2: remote wake up
 * Overview:        The USB sleep and wake up
 * Note:            None
 *****************************************************************************/
 unsigned char USB_WakeUp(void )
 {
    int delay_count;
   
    asm STOP;                                /*MCU enter into stop3*/
    
    Usb_Device_State = CONFIGURED_STATE;
    
    if(! USBCTL0_LPRESF)                  /*Is other interrupt bring the MCU out of STOP3*/
    {

       if(Usb_Stat.BitCtl.RemoteWakeup == 1) /*Is remote wakeup supported*/
         return 2;                           /*will enable remote wakeup*/
       else
         return 0;
    }                                        /*tell MCU it should enter STOP3 again*/
    else 
    {                                        /*add delay to filter the noise */
        delay_count = 50;               
        do
         {
           delay_count--;
           __RESET_WATCHDOG();
         }while(delay_count);        
        
        if(INTSTAT_RESUMEF)                  /*The resume signal has engouh time to set RESUMEF bit*/
        {
            return 1;                        /*USB resume*/
        }
        else
            return 0;                        /*resume is caused by noise*/
        
     }
}      
