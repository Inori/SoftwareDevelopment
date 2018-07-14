/*-----------------------------------------------
  FileName: uart.c
  Author: Bingkan
  Date: 2013/12/24
  Version: 1.0
  Describe: 串口发送数据
------------------------------------------------*/

#include"uart.h"

void SendChar(char c)
{
	SBUF = c;
	while(!TI);
	TI = 0;
}

//串口发送数据
void SendMsg(char *s)
{
	unsigned int i = 0;
	ES = 0;       /* 关闭串口中断*/
	while(1)
	{
		if(s[i] == 0)
			break;
		SendChar(s[i]);
		i++;
	}
	ES = 1;       /*打开串口中断*/
}
