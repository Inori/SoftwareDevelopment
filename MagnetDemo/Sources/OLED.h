#ifndef _LQOLED_H
#define _LQOLED_H
#include "derivative.h"
 void initLCD(void);
 void LCD_CLS(void);
 void LCD_P6x8Str(byte x,byte y,byte ch[]);
 void LCD_Fill(byte dat);
 void LCD_PutROW(unsigned char x,unsigned char y,unsigned char data1,unsigned char data2)  ;
#endif