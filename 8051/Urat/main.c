/*-----------------------------------------------
  FileName: main.c
  Author: Bingkan
  Date: 2013/12/24
  Version: 1.0
  Describe: 全局调用
------------------------------------------------*/

#include "include.h" 

//接受数据存储变量。串口收到数据后自动存储1字节到此变量
unsigned int result = 0;
//数码管显示的数字
unsigned int disnum = 1234;

void main (void)
{
	EA    = 0;                  //关闭总中断
	ALL_Init();
    EA    = 1;                  //打开总中断
    
	
	while (1)                   //主循环不做任何动作
    {
		//跑马灯演示，真正用时请关闭
		loopled();
    }
}



