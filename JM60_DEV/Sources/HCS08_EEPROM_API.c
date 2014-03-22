/** 
* Copyright (c) 2006, Freescale Semiconductor 
* File name : HCS08_EEPROM_API.c 
* CPU : MC9S08 
* Compiler: CodeWarrior v5.1 08C Compiler 
* 
* Author : Jerry Shi (b01136@freescale.com) 
* 
*          Modified by Bingkan Wang (811197881@qq.com), 03/21/2014
* 
* Description : This file includes definition for subroutines that  
*               emulating EEPROM with FLASH memory on HCS08 MCU. 
*                             
* History :    
* 10/08/2006  : API works on MC9S08QG8CDTE
* 03/21/2014  : Add 2 APIs, works on MC9S08JM60 
*/ 
 
#include <hidef.h> /* for EnableInterrupts macro */ 
#include <stdio.h>  /* for memcpy() */ 
#include "derivative.h" /* include peripheral declarations */ 
#include "hcs08_eeprom_api.h" 
 
 
#pragma MESSAGE DISABLE C4001   // condition always FALSE 
 
 
static void flash_byte_prog(unsigned int _addr, unsigned char _data); 
static void flash_page_erase(unsigned int _addr); 
static unsigned char flash_byte_read(unsigned int _addr); 
 
const unsigned char __NVPROT @ 0xFFBD = HCS08_FLASH_PROTECTION;

//record user's data length
unsigned int user_data_len; 
 
/* 
** =================================================================== 
**     Method      :  HCS08_EE_Init  
** 
**     Description : 
**         fFCLK initialization code for FLASH programming and erasing. 
**         fFCLK must be within the range from 150 kHz to 200 kHz.  
**         After this initialization methdo, fFCLK is always arround  
**         175 kHz, which is within the range of specification. 
**     Patameters: 
**          no. 
**     Return Values: 
**          no 
** =================================================================== 
*/ 
void HCS08_EE_Init(void) 
{ 
/**  
** fFCLK setting 
** FCDIV: 7 6 5 4 3 2 1 0 
**        | | |          
**        | | +DIV[5:0]: fFCLK = fBUS / (8exp(PRDIV8) * DIV +1) 
**        | +PRDIV8: Prescale by 8, 1: input to the FLASH clock divider is **                    
the bus rate clock divided by 8. 
**        +DIVLD: divisor load status, 1: has been written since reset. 
*/ 


 
if (!FCDIV_DIVLD) 
   { 
      if (HCS08_BUS_FREQUENCY >= 12000) 
      { 
          FCDIV = (((HCS08_BUS_FREQUENCY / (8*175))) | 0x40) - 1; 
      } 
      else 
      { 
          FCDIV = (HCS08_BUS_FREQUENCY / 175) - 1; 
      } 
   } 
} 
 
/* 
** =================================================================== 
**     Method      :  HCS08_EE_WriteRecord  
** 
**     Description : 
**         this function writes the whold data record to the next  
**         available space block of FLASH, also the Record Has Been 
**         used flag before the data. If the current page will be full,  
**         it erases the next page. 
**     Patameters: 
**          unsigned char *.    -- point to the source data 
**     Return Values: 
**          no 
** =================================================================== 
*/ 
void HCS08_EE_WriteRecord(unsigned char *src) 
{ 
    unsigned int i, j; 
    unsigned char flag_page_switch; 
    unsigned int ee_current_page, ee_next_page; 
 
    flag_page_switch = 0; 
    ee_current_page = EEPROM_START_ADDR; 
    ee_next_page = EEPROM_START_ADDR + EEPROM_PAGE_SIZE; 
 
    // find the current page 
    if (FLAG_CURENT_PAGE != 
          flash_byte_read(ee_current_page + EEPROM_PAGE_SIZE - 1)) 
    { 
        ee_current_page = EEPROM_START_ADDR + EEPROM_PAGE_SIZE; 
        ee_next_page = EEPROM_START_ADDR; 
    } 
 
    for (;;) 
    { 
        for (i = 0; 
             i < PAGE_REUSE_TIMES * EEPROM_DATA_LEN; 
             i = i + EEPROM_DATA_LEN + 1) 
        { 
            if (FLAG_RECORD_USED != flash_byte_read(ee_current_page + i)) 
            { 
                // this record space is empty 
                // wite flag to mark this record is used 
                flash_byte_prog(ee_current_page + i, FLAG_RECORD_USED); 
                // then write the record data to the following addresses 
                for (j = 0; j < EEPROM_DATA_LEN; j++) 
                { 
                    flash_byte_prog(ee_current_page + 1 + i + j, *(src + j)); 
                } 
 
                // do we need to switch to next page? 
                if (flag_page_switch) 
                    flash_page_erase(ee_next_page); 
                return; 
            } 
        } 
 
        // all the records are visited now, but does not find the current record 
        // page switch 
        j = ee_next_page; 
        ee_next_page = ee_current_page; 
        ee_current_page = j; 
        flag_page_switch = 1; 
        // mark the current page 
        flash_byte_prog((ee_current_page + EEPROM_PAGE_SIZE -1),      
FLAG_CURENT_PAGE); 
    } 
 
    return; 
} 
 
/* 
** =================================================================== 
**     Method      :  HCS08_EE_ModifyByte  
** 
**     Description : 
**         this function find the current data record from the current  
**         page, and modifies the specified byte of the record. 
**     Patameters: 
**          unsigned int *.     -- index of target byte to be modified in the record 
**          unsigned char *.    -- source data 
**     Return Values: 
**          no 
** =================================================================== 
*/ 
void HCS08_EE_ModifyByte(unsigned int index, unsigned char data) 
{ 
    unsigned char buff[EEPROM_DATA_LEN]; 
 
    HCS08_EE_ReadRecord(buff); 
    buff[index] = data; 
    HCS08_EE_WriteRecord(buff); 
 
    return; 
}

/* 
** =================================================================== 
**     Method      :  HCS08_EE_WriteData  
** 
**     Description : 
**         this function write data array to eeprom. 
**     Patameters: 
**          unsigned char* *.     -- address of the array to be wrote 
**          unsigned int   *.     -- length of array
**     Return Values: 
**          no 
** =================================================================== 
*/
void HCS08_EE_WriteData(unsigned char *src, unsigned int srclen)
{
   unsigned int idx = 0;
   
   user_data_len = srclen;
   if(user_data_len > EEPROM_DATA_LEN) 
      return;
   
   for(idx=0; idx < srclen; idx++)
   {
      HCS08_EE_ModifyByte(idx, src[idx]);
   }
}

/* 
** =================================================================== 
**     Method      :  HCS08_EE_ReadData  
** 
**     Description : 
**         this function read data array written by HCS08_EE_WriteData function. 
**     Patameters: 
**          unsigned char* *.     -- address of the array to hold result 
**          unsigned int   *.     -- length of array
**     Return Values: 
**          no 
** =================================================================== 
*/
void HCS08_EE_ReadData(unsigned char *dst, unsigned int dstlen)
{
   unsigned int idx = 0;
   
   if(dstlen > user_data_len) 
      return;
   
   for(idx=0; idx < dstlen; idx++)
   {
      dst[idx] = HCS08_EE_ReadByte(idx);
   }
}
 
/* 
** =================================================================== 
**     Method      :  HCS08_EE_ReadRecord  
** 
**     Description : 
**         this function finds the current record in the current page, 
**         then reads the all the data in the record to a buffer. 
**     Patameters: 
**          unsigned char *.    -- point to the dest buffer 
**     Return Values: 
**          no 
** =================================================================== 
*/ 
void HCS08_EE_ReadRecord(unsigned char *dest) 
{ 
    unsigned int i, j; 
    unsigned int ee_current_page; 
 
    ee_current_page = EEPROM_START_ADDR; 
 
    // find the current page 
    if (FLAG_CURENT_PAGE != flash_byte_read(ee_current_page + EEPROM_PAGE_SIZE - 1)) 
        ee_current_page = EEPROM_START_ADDR + EEPROM_PAGE_SIZE; 
 
    // visit all the records till find out the 'current' record          
    for (i = 0; 
         i < (PAGE_REUSE_TIMES - 1) * EEPROM_DATA_LEN; 
         i+= EEPROM_DATA_LEN + 1) 
    { 
        if (FLAG_RECORD_USED != flash_byte_read(ee_current_page + 1 + i + 
EEPROM_DATA_LEN)) 
            break; 
    } 
 
    // when the 'current' record is found, read data of it 
    for (j = 0; j < EEPROM_DATA_LEN; j++) 
        *(dest + j) = flash_byte_read(ee_current_page + 1 + i + j); 
 
    return; 
} 
 
/* 
** =================================================================== 
**     Method      :  HCS08_EE_ReadByte  
** 
**     Description : 
**         this function find the current data record from the current  
**         page, and reads the specified byte of the record. 
**     Patameters: 
**          unsigned int *.     -- index of target byte to be read in the record 
**     Return Values: 
**          unsigned char *.    -- data 
** =================================================================== 
*/ 
unsigned char HCS08_EE_ReadByte(unsigned int index) 
{ 
    unsigned int i; 
    unsigned int ee_current_page; 
 
    ee_current_page = EEPROM_START_ADDR; 
 
    // find the current page 
    if (FLAG_CURENT_PAGE != flash_byte_read(ee_current_page + EEPROM_PAGE_SIZE - 1)) 
        ee_current_page = EEPROM_START_ADDR + EEPROM_PAGE_SIZE; 
 
    // visit all the records till find out the 'current' record          
    for (i = 0; 
         i < (PAGE_REUSE_TIMES - 1) * EEPROM_DATA_LEN; 
         i+= EEPROM_DATA_LEN + 1) 
    { 
        if (FLAG_RECORD_USED != flash_byte_read(ee_current_page + 1 + i + 
EEPROM_DATA_LEN)) 
            break; 
    } 
 
    // when break, the current record is found 
    return flash_byte_read(ee_current_page + i + 1 + index); 
} 
 
/* ===================================================================*/ 
// Array of opcode instructions of the Erase/Program function in  
// the HCS08 family  
// size = 40 bytes  
const unsigned char ROM_PGM[] = 
{ 
  0x9E, 0xFE, 0x03, // LDHX 3,SP   ; get the target address '_addr' 
/* PC is pushed when calling, 2 bytes used (SP is decreased by 2) 
** SP is now pointing to the next available unit 
** thus '_addr' is at [SP+3] 
*/ 
  0xF7,               // STA ,X      ; store 'data' to 'addr'  
  0xA6, 0x20,           // LDA #$20    ; load FCMD, CMD should be changed before calling 
  0xC7, 0x18, 0x26,     // STA $1826   ; FCMD 
  0x45, 0x18, 0x25,     // LDHX #$1825 ; address of FSTAT 
  0xF6,                   // LDA ,X       ; load FSTAT to A 
  0xAA, 0x80,            // ORA #$80     ; set FCBEF to lunch the command 
  0xF7,                   // STA ,X       ; store back to FSTAT 
  0x9D,                   // NOP           ; wait for 4 cycles 
  0x9D,                   // NOP 
  0x9D,                   // NOP 
  0x9D,                   // NOP 
  0x45, 0x18, 0x25,     // LDHX #$1825 ; check FACCERR and FPVIOL 
  0xF6,                   // LDA ,X 
  0xA5, 0x30,            // BIT #$30 
  0x27, 0x04,            // BEQ *+6     ; branch to label_1 
  0xA6, 0xFF,            // LDA #$FF    ; return 0xFF if flag error 
  0x20, 0x07,            // BRA *+9     ; branch to label_2 
//label_1:   
  0xC6, 0x18, 0x25,   // LDA $1825   ; wait for command to be completed 
  0xA5, 0x40,            // BIT #$40 
  0x27, 0xF9,            // BEQ *-5     ; branch to label_1 
//label_2:   
  0x81                    // RTS          ; return 
}; 
/* 
** =================================================================== 
**     Method      :  flash_byte_prog  
** 
**     Description : 
**         flash_byte_prog programs one byte of data to the specified  
**         address of FLASH. this is an internal method only used by API. 
**     Patameters: 
**          unsigned int,       -- target address to be programmed 
**          unsigned char.      -- data to be programmed 
**     Return Values: 
**          no 
** =================================================================== 
*/ 
static 
void flash_byte_prog(unsigned int _addr, unsigned char _data) 
{ 
  unsigned char _pgm[sizeof(ROM_PGM) + 2]; 
 
  // save '_addr' 
  _pgm[0] = (unsigned char)((_addr & 0xff00) >> 8); 
  _pgm[1] = (unsigned char)(_addr & 0x00ff); 
 
  // copy the code to stack (local variable _pgm[])  
  (void)memcpy(_pgm + 2, ROM_PGM, sizeof(ROM_PGM)); 
 
  _pgm[7] = FLASH_CMD_BYTEPROG; // change FCMD value 
 
  DisableInterrupts; 
 
  if (FSTAT&0x10){               // Check to see if FACCERR is set 
      FSTAT = FSTAT | 0x10;     // write a 1 to FACCERR to clear 
  } 
  __asm 
  { 
    LDA _data     // pass '_data' thru A 
    TSX           // transfer SP to HX 
    JSR 2,X       // first 2 bytes of _pgm[] are '_addr' 
  } 
  EnableInterrupts; 
} 
 
/* 
** =================================================================== 
**     Method      :  flash_page_erase  
** 
**     Description : 
**         flash_page_erase erases one page that contains the specified  
**         address of FLASH. this is an internal method only used by API. 
**     Patameters: 
**          unsigned int.    -- target address to be erased 
**     Return Values: 
**          no 
** =================================================================== 
*/ 
static 
void flash_page_erase(unsigned int _addr) 
{ 
  unsigned char _pgm[sizeof(ROM_PGM) + 2]; 
 
  // save '_addr' 
  _pgm[0] = (unsigned char)((_addr & 0xff00) >> 8); 
  _pgm[1] = (unsigned char)(_addr & 0x00ff); 
 
  // copy codes to stack (local variable _pgm[]) 
  (void)memcpy(_pgm + 2, ROM_PGM, sizeof(ROM_PGM)); 
 
  _pgm[7] = FLASH_CMD_PAGEERASE;// change FCMD value 
 
  DisableInterrupts; 
  if (FSTAT&0x10){              // Check to see if FACCERR is set 
      FSTAT = FSTAT | 0x10;     // write a 1 to FACCERR to clear 
  } 
  __asm 
  { 
    TSX           // transfer SP to HX 
    JSR 2,X       // first 2 bytes of _pgm[] are '_addr' 
  } 
  EnableInterrupts; 
} 
 
/* 
** =================================================================== 
**     Method      :  flash_byte_read  
** 
**     Description : 
**         flash_byte_read reads one byte data from the specified  
**         address of FLASH. this is an internal method only used by API. 
**     Patameters: 
**          unsigned int.         -- target address to be erased 
**     Return Values: 
**          unsigned char.        -- the data 
** =================================================================== 
*/ 
static 
unsigned char flash_byte_read(unsigned int _addr) 
{ 
    return *(unsigned char *)_addr; 
} 