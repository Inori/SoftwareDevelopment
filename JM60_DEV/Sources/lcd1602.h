#ifndef LCD1602_H
#define LCD1602_H

#include "include.h"

#define   LCD_CLR_RET				0x01
#define	  LCD_RET						0x02
#define	  LCD_ENTRY_MODE		0x04
#define		ACInc						  0x02
#define		ScreenShift			  0x01
#define	  LCD_ONOFF_SET			0x08
#define		DisplayOn				  0x04
#define		CursorOn				  0x02
#define		CursorBlankingOn	0x01	
#define	  LCD_CURSOR_DISPLAY_SHIFT	0x10
#define		ShiftScreen				0x08
#define		ShiftToRight			0x04
#define	  LCD_FUNCTION_SET	0x38

 //001DLNF00,定义8位接口，两行显示，字体为5*8
#define   LCD_CGRAM_ADD_SET	0x40
#define	  LCD_DDRAM_ADD_SET	0x80
////////////////////////////////////////////////////////////////////////////////////////////
#define LCD_E         PTFD_PTFD1  //定义所用到的管脚
#define LCD_RS        PTFD_PTFD4
#define LCD_RW        PTFD_PTFD0
#define LCD_DATAPORT  PTED
#define LCD_DATA_DIR  PTEDD
#define	LCD_BUSY_CHECK() while((LCD_STATUS_READ()&0x80) == 0x80 ){;}

void PORT_INIT(void);
void LCD_COMMAND_WRITE(unsigned char LCD_COMMAND);
void LCD_DATA_WRITE(unsigned char LCD_DATA);
void LCD_CLS_ALL();
void LCD_SET_ADDRESS(unsigned char DISP_PAGE,unsigned char DISP_COL);
void LCD_INIT();
unsigned char LCD_STATUS_READ(void);
unsigned char LCD_DATA_READ(void);
void LCD_Print(unsigned char DISP_PAGE,unsigned char DISP_COL,unsigned char* p);

extern unsigned char const Menu1[];
extern unsigned char const Text[];
extern unsigned char const CGRAM_0[8]; 



#endif
	
