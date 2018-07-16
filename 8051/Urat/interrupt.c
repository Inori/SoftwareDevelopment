/*-----------------------------------------------
  FileName: interrupt.c
  Author: Bingkan
  Date: 2013/12/24
  Version: 1.0
  Describe: 所有中断子程序
------------------------------------------------*/

#include "include.h"
//定时器中断
void Timer0() interrupt 1 using 1
{
	TL0 = T1MS; 
	TH0 = T1MS >> 8;
	display(disnum);
}

void key_scan0()   interrupt 0    //外部中断 0	键盘第6键
{	
	if(disnum>=0 && disnum <= 9999)	
	{					
		disnum++;
	}
	else
	{
		disnum = 0;
	}
}

void key_scan1()   interrupt 3    //外部中断1 	键盘第5键
{						
	if(disnum>=0 && disnum <= 9999)	
	{					
		disnum--;
	}
	else
	{
		disnum = 0;
	}
}

//串口中断程序
void UART_SER (void) interrupt 4 //串行中断服务程序
{
   if(RI)                        //判断是接收中断产生
   {
	  RI=0;                      //标志位清零
	  result = SBUF;                 //读入缓冲区的值

	  //下面两句debug用
	  //SBUF = result;
	  //SendMsg("Fuck\n");
   }
   if(TI)                        //如果是发送标志位，清零
	TI=0;
} 