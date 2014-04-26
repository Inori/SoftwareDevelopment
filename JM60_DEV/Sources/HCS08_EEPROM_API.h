/** 
* Copyright (c) 2006, Freescale Semiconductor 
* File name : HCS08_EEPROM_API.h 
* CPU : MC9S08 
* Compiler: CodeWarrior v5.1 08C Compiler 
* 
* Author : Jerry Shi (b01136@freescale.com)
*
*          Modified by Bingkan Wang (811197881@qq.com), 03/21/2014
* 
* Description : This head file includes definition for subroutines that  
*               emulating EEPROM with FLASH memory on HCS08 MCU. 
*                             
* History :    
* 10/08/2006  : API works on MC9S08QG8CDTE
* 03/21/2014  : Add 2 APIs, works on MC9S08JM60 
*/ 
#ifndef __HCS08_EEPROM_API_H 
#define __HCS08_EEPROM_API_H 
 
/** 
**  Below macros define some nesseccsry data for EEPROM emulation. 
**  Users MUST modify those macros according to their own application. 
**  PRM file also should be modified to update the ROM address and 
**  reserve EEPROM address (2 pages). 
**  STACKSIZE must be carefully reviewed because the APU functions  
**  uses many stack size. Do-on-stack code size is 43 bytes. 
**  Function HCS08_EE_ModifyByte() uses as much as EEPROM_DATA_LEN 
**  bytes of stack to hold the data buffer.  
**/ 
 
/**  
** EEPROM data is treated as fixed-size record.  
**     FLAG, DATA0, DATA1, DATA2, ..., DATAn 
**  
**/ 
#define EEPROM_DATA_LEN         0x20 
/** 
** Refer to the PRM file of your project and define HCS08_FLASH_START_ADDR 
** accordingly. Also the PRM file should be modified to specify the new ROM 
** address. 2 pages are used for EEPROM emulatio, thus ROM = ROM + 0x200. 
**/ 
#define HCS08_FLASH_START_ADDR  0x10B0 
/** 
** Page size of HCS08 is 0x200 
**/ 
#define HCS08_FLASH_PAGE_SIZE   0x200 
/** 
** CPU bus frequncy, modify it according to your system configuration. 
** Unit is kHz. For example, 4 MHz is defined as 4000 (kHz) 
**/ 
#define HCS08_BUS_FREQUENCY     9200    
/** 
** FLASH block protection. Default is off.  
** It is stongly to turn it on to protect the code space of CPU. 
**/ 
#define HCS08_FLASH_PROTECTION  0xFF 
 
/** 
**  Possible return values of operation result. 
**/ 
#define HCS08_EE_ERROR_OK           0x00   
#define HCS08_EE_ERROR_UNKOWN       0xFF 
#define HCS08_EE_ERROR_OUT_OF_RANGE 0x01 
 
/** 
**  Below macros are defined for API use only.  
**  Do NOT modify it.  
**/ 
#define FLAG_CURENT_PAGE  0xAA 
#define FLAG_RECORD_USED  0x55 
 
#define FLASH_CMD_BYTEPROG  0x20 
#define FLASH_CMD_PAGEERASE 0x40     
 
#define EEPROM_START_ADDR HCS08_FLASH_START_ADDR 
#define EEPROM_PAGE_SIZE  HCS08_FLASH_PAGE_SIZE 
/** 
** how many times one page can be re-used 
** (HCS08_FLASH_PAGE_SIZE - 1) / (EEPROM_DATA_LEN + 1) 
**/ 
#define PAGE_REUSE_TIMES (HCS08_FLASH_PAGE_SIZE - 1) / (EEPROM_DATA_LEN + 1)  
 
/** 
**  Functions declarations of API.  
**/ 
void HCS08_EE_Init(void); 
void HCS08_EE_WriteRecord(unsigned char *src); 
void HCS08_EE_ReadRecord(unsigned char *dest); 
void HCS08_EE_ModifyByte(unsigned int index, unsigned char data); 
unsigned char HCS08_EE_ReadByte(unsigned int index);

void HCS08_EE_WriteData(unsigned char *src, unsigned int srclen);
void HCS08_EE_ReadData(unsigned char *dst, unsigned int dstlen); 
 
 
#endif 