/*-----------------------------------------------
  FileName: uart.c
  Author: Bingkan
  Date: 2013/12/24
  Version: 1.0
  Describe: ���ڷ�������
------------------------------------------------*/

#include"uart.h"

void SendChar(char c)
{
	SBUF = c;
	while(!TI);
	TI = 0;
}

//���ڷ�������
void SendMsg(char *s)
{
	unsigned int i = 0;
	ES = 0;       /* �رմ����ж�*/
	while(1)
	{
		if(s[i] == 0)
			break;
		SendChar(s[i]);
		i++;
	}
	ES = 1;       /*�򿪴����ж�*/
}
