/*-----------------------------------------------
  FileName: interrupt.c
  Author: Bingkan
  Date: 2013/12/24
  Version: 1.0
  Describe: �����ж��ӳ���
------------------------------------------------*/

#include "include.h"
//��ʱ���ж�
void Timer0() interrupt 1 using 1
{
	TL0 = T1MS; 
	TH0 = T1MS >> 8;
	display(disnum);
}

void key_scan0()   interrupt 0    //�ⲿ�ж� 0	���̵�6��
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

void key_scan1()   interrupt 3    //�ⲿ�ж�1 	���̵�5��
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

//�����жϳ���
void UART_SER (void) interrupt 4 //�����жϷ������
{
   if(RI)                        //�ж��ǽ����жϲ���
   {
	  RI=0;                      //��־λ����
	  result = SBUF;                 //���뻺������ֵ

	  //��������debug��
	  //SBUF = result;
	  //SendMsg("Fuck\n");
   }
   if(TI)                        //����Ƿ��ͱ�־λ������
	TI=0;
} 