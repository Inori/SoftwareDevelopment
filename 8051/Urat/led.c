/*-----------------------------------------------
  FileName: led.c
  Author: Bingkan
  Date: 2013/12/24
  Version: 1.0
  Describe: 数码管显示程序
------------------------------------------------*/
#include "led.h"


//显示位置
unsigned char l_posit=0;	
// 显示段码值0123456789ABCDEF
unsigned char const ledtbl[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e};

//显示函数，参数为显示内容
void display(unsigned int num)
{
	P0=0XFF;			
	switch(l_posit){
	case 0:		//选择千位数码管，关闭其它位
		LED4=0;
		LED3=1;
		LED2=1;	
		LED1=1;
		P0=ledtbl[num/1000];	//输出显示内容
		break;
	case 1:		//选择百位数码管，关闭其它位
		LED4=1;
		LED3=0;	
		LED2=1;		
		LED1=1;
		P0=ledtbl[num%1000/100];
		break;
	case 2:		//选择十位数码管，关闭其它位
		LED4=1;
		LED3=1;	
		LED2=0;		
		LED1=1;
		P0=ledtbl[num%100/10];
		break;
	case 3:		//选择个位数码管，关闭其它位
		LED4=1;
		LED3=1;	
		LED2=1;		
		LED1=0;
		P0=ledtbl[num%10];
		break;
	}
	l_posit++;		//每调用一次将轮流显示一位
	if(l_posit>3)
		l_posit=0;	
}




