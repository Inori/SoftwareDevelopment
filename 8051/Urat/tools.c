/*-----------------------------------------------
  FileName: tools.c
  Author: Bingkan
  Date: 2013/12/24
  Version: 1.0
  Describe: ���ߺ���
------------------------------------------------*/

#include "tools.h"


//�������ʾ����
void loopled(void)
{
	  delay(150);
	  P2 <<= 1;      //����һλ
	  P2 |= 0x01;    //���һλ��1
	  if(P2 == 0x7f)
      { 
	     delay(150);
	     P2=0xfe;
      }
}

//��ʱ�ӳ���
void delay(unsigned int cnt) 
{
	unsigned int i=0, j=0;
	for(i=0; i< cnt; i++)
	{
		for(j=0; j<200; j++);
	}
}


