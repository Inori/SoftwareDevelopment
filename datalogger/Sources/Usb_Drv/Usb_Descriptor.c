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
 * File name   : Usb_Description.c
 * Description : This file defines the USB descriptor and intizlize them 
 * History     :
 * 04/01/2007  : Initial Development
 * 
 *************************************************************************************************/
#include <stddef.h>
#include "typedef.h"
#include "Usb_Descriptor.h"
#include "Usb_Config.h"
#include "Usr_Ep_Handler.h"

 
const USB_DEV_DSC Device_Dsc =
{    
    sizeof(USB_DEV_DSC),    /* Size of this descriptor in bytes     */
    DSC_DEV,                /* DEVICE descriptor type               */
    0x0002,                 /* USB Spec Release Number in BCD format*/
    0x00,                   /* Class Code                           */
    0x00,                   /* Subclass code                        */
    0x00,                   /* Protocol code                        */
    EP0_BUFF_SIZE,          /* Max packet size for EP0,             */
    0xA215,                 /* Vendor ID		//little-endian for USB */
    0x3600,                 /* Product ID:	//little-endian for USB */
    0x0002,                 /* Device release number in BCD format  */
    0x00,                   /* Manufacturer string index            */
    0x02,                   /* Product string index                 */
    0x00,                   /* Device serial number string index    */
    0x01                    /* Number of possible configurations    */
};

 const USB_CFG  Cfg_01={
    {
    sizeof(USB_CFG_DSC),    /* Size of this descriptor in bytes   */
    DSC_CFG,                /* CONFIGURATION descriptor type      */
    (word) ((sizeof(USB_CFG)<<8) | (sizeof(USB_CFG)>>8)),          /* Total length of data for this cfg*/
    1,                      /* Number of interfaces in this cfg   */
    1,                      /* Index value of this configuration  */
    0,                      /* Configuration string index         */
    _DEFAULT|_REMOTEWAKEUP, /* Attributes, see usbdefs_std_dsc.h  */
    50                      /* Max power consumption (2X mA)      */
    },
    /* Interface Descriptor */
    {
    sizeof(USB_INTF_DSC),   /* Size of this descriptor in bytes   */
    DSC_INTF,               /* INTERFACE descriptor type          */
    0,                      /* Interface Number                   */
    0,                      /* Alternate Setting Number           */
    4,                      /* Number of endpoints in this intf   */
    0,                      /* Class code                         */
    0,                      /* Subclass code                      */
    0,                      /* Protocol code                      */
    0                       /* Interface string index             */
    },
     /* Endpoint Descriptor*/
    {sizeof(USB_EP_DSC),DSC_EP,_EP01_OUT,_BULK,(word)(UEP1_SIZE << 8),20} ,
    {sizeof(USB_EP_DSC),DSC_EP,_EP02_IN,_BULK,(word)(UEP2_SIZE << 8),20} ,
    {sizeof(USB_EP_DSC),DSC_EP,_EP03_OUT,_BULK,(word)(UEP3_SIZE << 8),20} ,
    {sizeof(USB_EP_DSC),DSC_EP,_EP05_IN,_INT,(word)(UEP5_SIZE << 8),1}
};

struct
{
  byte bLength;
  byte bDscType;
  word string[1];
} const sd000 
= {sizeof(sd000),DSC_STR,0x0904};

struct
{
  byte bLength;
  byte bDscType;
  word string[25];
} const sd001
  ={
    sizeof(sd001),DSC_STR,
    'F','r','e','e','s','c','a','l','e',' ',
    'T','e','c','h','n','o','l','o','g','y',' ','I','n','c','.'
   }; 
       
struct
{
  byte bLength;
  byte bDscType;
  word string[33];
} const sd002
={
    sizeof(sd002),DSC_STR,
    ' ',' ','J','M','6','0',' ','F','S',' ','U','S','B',' ',
    'D','e','m','o',' ','B','o','a','r','d',' ','(','C',')',
    ' ','2','0','0','7'
   }; 
   

    
unsigned char* Str_Des[] = 
    { (unsigned char*)&sd000,(unsigned char*)&sd001,(unsigned char*)&sd002};   

unsigned char* Cfg_Des[] =
    {(unsigned char*)&Cfg_01,(unsigned char*)&Cfg_01 };
    
pFunc Class_Req_Handler[1] = { NULL };


