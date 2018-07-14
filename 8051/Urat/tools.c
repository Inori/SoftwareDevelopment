/*-----------------------------------------------
  FileName: tools.c
  Author: Bingkan
  Date: 2013/12/24
  Version: 1.0
  Describe: 工具函数
------------------------------------------------*/

#include "tools.h"


//跑马灯演示程序
void loopled(void)
{
	  delay(150);
	  P2 <<= 1;      //左移一位
	  P2 |= 0x01;    //最后一位补1
	  if(P2 == 0x7f)
      { 
	     delay(150);
	     P2=0xfe;
      }
}

//延时子程序
void delay(unsigned int cnt) 
{
	unsigned int i=0, j=0;
	for(i=0; i< cnt; i++)
	{
		for(j=0; j<200; j++);
	}
}


