#include "lcd1602.h"


//由于CGROM中定义字符的位置和字符的ASCII码相同，因此可以直接赋值
unsigned char const Menu1[] = "slchen@tju.edu.c";
//显示三个对勾+12345+三个对勾
unsigned char const Text[] ={0x01,0x01,0x01,0x31,0x32,0x33,0x34,0x35,0x01,0x01,0x01,0x00};

unsigned char const CGRAM_0[8] = {0x00,0x00,0x01,0x02,0x14,0x08,0x00,0x00};

void time(uint t)
{
  uint i,j;
  for(i=0;i<t;i++)
  {
    for(j=0;j<100;j++);
  }
}

void PORT_INIT(void)
{
  PTFDD |=0x13; //F0,F1,F4为输出
  PTEDD = 0xFF; //PTE口作为输出；
}

void LCD_INIT()   //初始化
{	
   unsigned char i;
   
   PORT_INIT();
   
   LCD_COMMAND_WRITE(LCD_FUNCTION_SET);
	 time(30);                                                         
   LCD_COMMAND_WRITE(LCD_FUNCTION_SET);
	 time(6);                                                         
   LCD_COMMAND_WRITE(LCD_FUNCTION_SET);
	 time(6);                                                         
   LCD_COMMAND_WRITE(LCD_FUNCTION_SET);
	 LCD_BUSY_CHECK();
   //LCD_COMMAND_WRITE(LCD_ONOFF_SET|DisplayOn|CursorOn|CursorBlankingOn);
   LCD_COMMAND_WRITE(LCD_ONOFF_SET|DisplayOn); 
	 LCD_BUSY_CHECK();
	 LCD_COMMAND_WRITE(LCD_CLR_RET); 
	 LCD_BUSY_CHECK();
	 LCD_COMMAND_WRITE(LCD_ENTRY_MODE|ACInc);

   LCD_BUSY_CHECK();
   LCD_COMMAND_WRITE(LCD_CGRAM_ADD_SET+0x08);   
//将自定义字符写在DDRAM的0x01中
	for(i = 0;i<8;i++)
	{ 	
	  LCD_BUSY_CHECK();
		LCD_DATA_WRITE(CGRAM_0[i]);	
	}
}


void LCD_COMMAND_WRITE(unsigned char LCD_COMMAND)  //写命令函数
{
	LCD_E	=0;
	LCD_DATAPORT = LCD_COMMAND;
	LCD_RS 	= 0;	//命令或状态
  LCD_RW 	= 0;	//执行写操作
  asm ("nop; nop;nop;nop;"); //地址设置时间
 	LCD_E	= 1;
  asm ("nop; nop;nop;nop;"); //时钟脉宽控制
	LCD_E	=0;		//锁存数据
  asm ("nop; nop;nop;nop;");	//地址保持时间
  LCD_RW 	= 1;	//回到读状态
}

//读取液晶状态信息
unsigned char LCD_STATUS_READ()
{
	unsigned char status;
	LCD_E	=0;
	//LCD_DATAPORT = 0xff;//设置为端口读状态
	LCD_DATA_DIR = 0;
	
	LCD_RS 	= 0;	//命令或状态
  LCD_RW 	= 1;	//执行读操作
  asm ("nop; nop;nop;nop;"); //地址设置时间
	LCD_E	= 1;
  asm ("nop; nop;nop;nop;"); //时钟脉宽控制
      //数据建立时间
	status 	= LCD_DATAPORT;//读取数据
	LCD_E	= 0;		
  asm ("nop; nop;nop;nop;"); //地址保持时间
  LCD_RW 	= 1;	//回到读状态
	
	LCD_DATA_DIR = 0xFF;
	return status;
}


//写数据函数
void LCD_DATA_WRITE(unsigned char LCD_DATA)  
{
	LCD_E	=0;
	LCD_DATAPORT = LCD_DATA;
	LCD_RS 	= 1;	//数据操作
  LCD_RW 	= 0;	//执行写操作
  asm ("nop; nop;nop;nop;"); //地址设置时间
	LCD_E	= 1;
  asm ("nop; nop;nop;nop;"); //时钟脉宽控制
	LCD_E	=0;		//锁存数据
  asm ("nop; nop;nop;nop;"); //地址保持时间
  LCD_RW 	= 1;	//回到读状态
}



unsigned char LCD_DATA_READ(void)
{
	unsigned char ReadData;
	LCD_E	=0;
	//LCD_DATAPORT = 0xff;//设置为端口读状态
	LCD_DATA_DIR = 0;
	
	LCD_RS 	= 1;	//数据操作
  LCD_RW 	= 1;	//执行读操作
  asm ("nop; nop;nop;nop;"); //地址设置时间
	LCD_E	= 1;
  asm ("nop; nop;nop;nop;"); //时钟脉宽控制
	ReadData 	= LCD_DATAPORT;//读取数据
	LCD_E	= 0;		
  asm ("nop; nop;nop;nop;"); //地址保持时间
  LCD_RW 	= 1;	//回到读状态
	LCD_DATA_DIR = 0xFF;
	return ReadData;
}



void LCD_SET_ADDRESS(unsigned char DISP_PAGE,unsigned char DISP_COL)
{
	unsigned char col;
	col = DISP_COL;
	if( col > 15 ) col = 15;
	LCD_BUSY_CHECK();
	if(DISP_PAGE == 0)
		 LCD_COMMAND_WRITE(LCD_DDRAM_ADD_SET + col);  
 //写显示的位置
	else 	
	   LCD_COMMAND_WRITE(LCD_DDRAM_ADD_SET + 0x40 + col);
}



void LCD_CLS_ALL()   //清屏
{
	 LCD_BUSY_CHECK();
	 LCD_COMMAND_WRITE(LCD_CLR_RET); 
	 LCD_BUSY_CHECK();
}



void LCD_Print(unsigned char DISP_PAGE,unsigned char DISP_COL,unsigned char* p)
{
	unsigned char i,DisCnt;
	unsigned char col;
	col = DISP_COL;
	if(col>15)	
	  col = 15;
	
	DisCnt = 16-col;
	
	LCD_BUSY_CHECK();
	LCD_SET_ADDRESS(DISP_PAGE,DISP_COL);
	for(i = 0;i<DisCnt;i++)
	{
	 	LCD_BUSY_CHECK();
		if(p[i]!=0)					//字符串以0x00结尾
			LCD_DATA_WRITE(p[i]);	
		else break;         //如果一写完就跳出程序
	}	
}
