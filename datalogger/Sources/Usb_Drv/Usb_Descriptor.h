/*************************************************************************
 * DISCLAIMER                                                            *
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
 * File name   : UsbDescriptor.h
 *
 * Description : This file defines the USB standard descriptor 
 *
 * History     :
 * 04/01/2007  : Initial Development
 * 
 *************************************************************************************************/
#include "typedef.h"
#include "Usb_Config.h"


#ifndef USBDEFS_STD_DSC_H
#define USBDEFS_STD_DSC_H



/* Descriptor Types*/
#define DSC_DEV     0x01
#define DSC_CFG     0x02
#define DSC_STR     0x03
#define DSC_INTF    0x04
#define DSC_EP      0x05


 /* USB standard endpoint address format: dir:X:X:X:EP3:EP2:EP1:EP0*/
#define _EP01_OUT   0x01
#define _EP01_IN    0x81
#define _EP02_OUT   0x02
#define _EP02_IN    0x82
#define _EP03_OUT   0x03
#define _EP03_IN    0x83
#define _EP04_OUT   0x04
#define _EP04_IN    0x84
#define _EP05_OUT   0x05
#define _EP05_IN    0x85
#define _EP06_OUT   0x06
#define _EP06_IN    0x86

/* Endpoint transfer type*/
#define _CTRL       0x00            //control transfer
#define _ISO        0x01            //isochronous transfer
#define _BULK       0x02            //bulk transfer
#define _INT        0x03            //interrupt transfer

/*isochronous endpoint synchronization type */
#define _NS         0x00<<2         //No Synchronization
#define _AS         0x01<<2         //Asynchronous
#define _AD         0x02<<2         //Adaptive
#define _SY         0x03<<2         //Synchronous

/*sochronous Endpoint Usage Type */
#define _DE         0x00<<4         //Data endpoint
#define _FE         0x01<<4         //Feedback endpoint
#define _IE         0x02<<4         //Implicit feedback Data endpoint


/*USB device descriptor structure*/
typedef struct _USB_DEV_DSC
{
    byte bLength;
    byte bDscType;
    word bcdUSB;
    byte bDevCls;
    byte bDevSubCls;
    byte bDevProtocol;
    byte bMaxPktSize0;
    word idVendor;
    word idProduct;
    word bcdDevice;
    byte iMFR;
    byte iProduct;
    byte iSerialNum;
    byte bNumCfg;
} USB_DEV_DSC;


/*USB configuration descriptor structure*/
typedef struct _USB_CFG_DSC
{
    byte bLength;
    byte bDscType;
    word wTotalLength;
    byte bNumIntf;
    byte bCfgValue;
    byte iCfg;
    byte bmAttributes;
    byte bMaxPower;
} USB_CFG_DSC;

/* USB interface descriptor structure*/
typedef struct _USB_INTF_DSC
{
    byte bLength;
    byte bDscType;
    byte bIntfNum;
    byte bAltSetting;
    byte bNumEPs;
    byte bIntfCls;
    byte bIntfSubCls;
    byte bIntfProtocol;
    byte iIntf;
} USB_INTF_DSC;

/*USB endpoint descriptor structure*/
typedef struct _USB_EP_DSC
{
    byte bLength;
    byte bDscType;
    byte bEPAdr;
    byte bmAttributes;
    word wMaxPktSize;
    byte bInterval;
} USB_EP_DSC;



typedef struct _USB_CFG                            
{   
    USB_CFG_DSC     Cfg_Dsc_01;                  
    USB_INTF_DSC    Int_Dsc_Intf00Alt00;                
    USB_EP_DSC      Ep1_Dsc_Intf00Alt00;
    USB_EP_DSC      Ep2_Dsc_Intf00Alt00;  
    USB_EP_DSC      Ep3_Dsc_Intf00Alt00;
    USB_EP_DSC      Ep4_Dsc_Intf00Alt00;  
            
}USB_CFG;


extern unsigned char* Str_Des[];
extern unsigned char* Cfg_Des[];
extern const USB_CFG  Cfg_01;
extern const USB_DEV_DSC Device_Dsc;
extern pFunc Class_Req_Handler[1] ;


#endif 