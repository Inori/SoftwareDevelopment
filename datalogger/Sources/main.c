#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */
#include "Usb_Drv.h" 
#include "Usb_Config.h"
#include "adc.h"
#include "tpm.h"
#include "gpio.h"
#include "Usr_Task.h"


const byte NVOPT_INIT  @0x0000FFBF = 0x02;    /* vector redirect, flash unsecure*/
const byte NVPROT_INIT @0x0000FFBD = 0xFA;    /* 0xFC00-0xFFFF are protected  */

extern void _Startup(void);

/*Local function definition*/ 
void Init_Sys(void);
void Mcu_Init(void);
void Reset_Indicator(void);


interrupt void Dummy_ISR(void) {
  
}
 
 
void main(void) 
{

     
     MCGC2= 0x36;
     while(!MCGSC_OSCINIT) ;
     MCGC1 = 0xB8;
     while(MCGSC_IREFST);
     while(MCGSC_CLKST!=2);
     MCGC1 = 0x88;//   RDIV = 2; //0x90;  //RDIV = 4
     MCGC3 = 0x46;//   24±¶Æµ;  //0x44;//16±¶Æµ
     while(!MCGSC_PLLST);
     while(!MCGSC_LOCK);
     MCGC1 = 0x08;    //ÇÐ»»µ½PLL  0x10;
     while(MCGSC_CLKST!=3);
     
  PTBDD |= 0x03;
  
  
  Init_Sys();                         /*initial the system*/
  
  EnableInterrupts;                   /* enable interrupts */

   
  for(;;) 
  {
    __RESET_WATCHDOG();                /* feeds the dog  */ 
    
    Check_USB_Status();             /* Must use polling method*/
    
    if(Usb_Device_State == CONFIGURED_STATE)
      Usr_Task();                     /* User task--- LED transaction */
    
  } 
 }



/*****************************************************************************
 * Init_Sys: Initialize the system
 * Input: 	 None
 * Output:   None
 *
 ****************************************************************************/
void Init_Sys(void)
{
	Mcu_Init(); 
  //Init_GPIO();

  Reset_Indicator();

  //ADC_Init();
  //TPM_Init();

	Initialize_USBModule(); 
	
  return;
}

/*****************************************************************************
 * Function     MCU_Init
 * Overview:    Initialization of the MCU.
 *              It will configure the JM60 to disable STOP and COP Modules.
 *              It also set the MCG configuration and bus clock frequency.
 *
 * Input  : None
 *
 * Return : None
 ****************************************************************************/
void Mcu_Init()
{
	SOPT1 = 0xF3; 			  /*Enable the COP,  and enable the MCU stop mode*/
			                  /*  
                         *  0b00000010
                         *    ||||||||__ bit0:   
                         *    |||||||___ bit1: 
                         *    ||||||____ bit2: 
                         *    |||||_____ bit3: 
                         *    ||||______ bit4: 
                         *    |||_______ bit5: STOP Mode enable
                         *    ||________ bit6: COP configuration
                         *    |_________ bit7: COP configuration
                         */
	SOPT2 = 	0x00;
			                  /*  
                         *  0b00000010
                         *    ||||||||__ bit0:   ACIC  		ACMP output connect to TPM channel 0
                         *    |||||||___ bit1:   SPI2FE		SPI2 input filter enable
                         *    ||||||____ bit2:   SPI1FE		SPI1 input filter enable
                         *    |||||_____ bit3:   
                         *    ||||______ bit4: 
                         *    |||_______ bit5:   COPW COP Window
                         *    ||________ bit6:   COPCLKS	COP wactchdog clock select
                         *    |_________ bit7:   COPCLKS	COP wactchdog clock select
                         */
	SPMSC1 = 0x41;
			                  /*  
                         *  0b01000010
                         *    ||||||||__ bit0:   BGBE  		Bandgap Buffer enable
                         *    |||||||___ bit1:   0			
                         *    ||||||____ bit2:   LVDE		Low_Voltage detect enable
                         *    |||||_____ bit3:   LVDSE		Low_Voltage Detect in stop mode
                         *    ||||______ bit4:   LVDRE		Low_Voltage Detect Reset Enable
                         *    |||_______ bit5:   LVWIE		Low_Voltage Warning interrupt Enable
                         *    ||________ bit6:   LVWACK	Low-Voltage Warning Acknowledge
                         *    |_________ bit7:   LVWF		Low_Voltage Warning Flag
                         */
  SPMSC2 = 0x00;
		                  	/*  
                         *  0b01000010
                         *    ||||||||__ bit0:   PPDC  		Partlal Power Down Control
                         *    |||||||___ bit1:   0			
                         *    ||||||____ bit2:   PPDACK	Partlal Power Down Acknowledge
                         *    |||||_____ bit3:   PPDF		partlal Power Down Flag
                         *    ||||______ bit4:   LVWV		Low-Voltage Warning Voltage Select
                         *    |||_______ bit5:   LVDV		Low_Voltage Detect Voltage select
                         *    ||________ bit6:   0
                         *    |_________ bit7:   0
                         */
  
  /*IRQSC = 0x02;*/   /*enable the IRQ interrupt */
			
}

void Reset_Indicator(void) 
{
  int Loop = 5;
  int Counter;
  
  while(Loop) 
  {
    Counter = 60000;
    
    while(Counter) 
    {
       Counter --;
       __RESET_WATCHDOG();
    }
    Loop--;
    PTED_PTED2 = ~PTED_PTED2;
    
    
  } 
  
  PTED_PTED2 = 1;
  return;
}




void (* volatile const _UserEntry[])()@0xFBBC={
  0x9DCC,             /* asm NOP(9D), asm JMP(CC)*/
  _Startup
};
  
/* redirect vector 0xFFC0-0xFFFD to 0xFBC0-0xFBFD */
void (* volatile const _Usr_Vector[])()@0xFBC4= 
{
    RTC_ISR,          /* Int.no.29 RTC                (at FFC4)  */
    Dummy_ISR,        /* Int.no.28 IIC                (at FFC6)  */
    Dummy_ISR,        /* Int.no.27 ACMP               (at FFC8)  */
    ADC_ISR,          /* Int.no.26 ADC                (at FFCA)  */
    Dummy_ISR,        /* Int.no.25 KBI                (at FFCC)  */
    Dummy_ISR,        /* Int.no.24 SCI2 Transmit      (at FFCE)  */
    Dummy_ISR,        /* Int.no.23 SCI2 Receive       (at FFD0)  */
    Dummy_ISR,        /* Int.no.22 SCI2 Error         (at FFD2)  */
    Dummy_ISR,        /* Int.no.21 SCI1 Transmit      (at FFD4)  */
    Dummy_ISR,        /* Int.no.20 SCI1 Receive       (at FFD6)  */
    Dummy_ISR,        /* Int.no.19 SCI1 error         (at FFD8)  */
    Dummy_ISR,        /* Int.no.18 TPM2 Overflow      (at FFDA)  */
    Dummy_ISR,        /* Int.no.17 TPM2 CH1           (at FFDC)  */
    Dummy_ISR,        /* Int.no.16 TPM2 CH0           (at FFDE)  */
    TPM1_ISR ,        /* Int.no.15 TPM1 Overflow      (at FFE0)  */
    Dummy_ISR,        /* Int.no.14 TPM1 CH5           (at FFE2)  */
    Dummy_ISR,        /* Int.no.13 TPM1 CH4           (at FFE4)  */
    Dummy_ISR,        /* Int.no.12 TPM1 CH3           (at FFE6)  */
    Dummy_ISR,        /* Int.no.11 TPM1 CH2           (at FFE8)  */
    Dummy_ISR,        /* Int.no.10 TPM1 CH1           (at FFEA)  */
    Dummy_ISR,        /* Int.no.9  TPM1 CH0           (at FFEC)  */
    Dummy_ISR,        /* Int.no.8  Reserved           (at FFEE)  */
    USB_ISR,          /* Int.no.7  USB Statue         (at FFF0)  */
    Dummy_ISR,        /* Int.no.6  SPI2               (at FFF2)  */
    Dummy_ISR,        /* Int.no.5  SPI1               (at FFF4)  */
    Dummy_ISR,        /* Int.no.4  Loss of lock       (at FFF6)  */
    Dummy_ISR,        /* Int.no.3  LVI                (at FFF8)  */
    IRQ_ISR,          /* Int.no.2  IRQ                (at FFFA)  */
    Dummy_ISR,        /* Int.no.1  SWI                (at FFFC)  */
};



#pragma CODE_SEG Bootloader_ROM

void Bootloader_Main(void);

void _Entry(void) 
{ 
  /* Initialization I/O for enter bootloader mode  */
  PTGD = 0x00;			
  PTGDD = 0xF0;                   /*PTG0-3 used for KBI input*/
	PTGPE = 0x0F;		                /*Pull-up enable */
  
   // MCG clock initialization, fBus=24MHz
  MCGC2 = 0x36;
  while(!(MCGSC & 0x02));		      //wait for the OSC stable
  MCGC1 = 0x1B;
	MCGC3 = 0x48;
	while ((MCGSC & 0x48) != 0x48);	//wait for the PLL is locked
  
  
  /* Flash clock*/
  FCDIV=0x4E;                     /* PRDIV8=1; DIV[5:0]=14, flash clock should be 150-200kHz*/
                                  /* bus clock=24M, flash clock=fbus/8/(DIV[5:0]+1) */
  /* Mode Determination*/
  if(!PTGD_PTGD0)                 /* PTG0 is pressed*/
  {
    SOPT1 = 0x20;                 /* disable COP*/

    USBCTL0=0x44;                 /* Initialize on-chip regulator and pullup resistor*/
    Bootloader_Main();            /* Bootloader mode*/
  }
  else
    asm JMP _UserEntry;           /* Enter User mode*/
}


#pragma CODE_SEG default











