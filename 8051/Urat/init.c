/*-----------------------------------------------
  FileName: init.c
  Author: Bingkan
  Date: 2013/12/24
  Version: 1.0
  Describe: 所有模块初始化代码
------------------------------------------------*/
#include "init.h"

void Uart_Init(void)		//1200bps@12MHz
{
	SCON = 0x50;		//8位数据,可变波特率
	TMOD |= 0x20;		//设定定时器1为8位自动重装方式
	TH1 = 0xE6;	    	//设定定时器重装值
	TR1 = 1;	    	//启动定时器1
	IPH |= 0x10;        //中断优先级最高
	PS = 1;             //中断优先级最高
	ES = 1;             //打开串口中断
	TI = 0;
}

void Loop_Init(void)
{
	P2=0xfe;           //给初始化值 换算成二进制是1111 1110
}

void Int_Init(void)
{
	EX0=1;  		//开启外部中断 0
	IT0=0; 
	EX1=1;  		//开启外部中断 1
	IT1=0; 
}
void Timer_Init(void)
{
	TMOD |= 0x01;
	TL0 = T1MS; 
	TH0 = T1MS >> 8;
	TR0 = 1; 
	ET0 = 1;
}

void ALL_Init(void)
{
	Loop_Init();
	Uart_Init();
	Timer_Init();
	Int_Init();
} 
