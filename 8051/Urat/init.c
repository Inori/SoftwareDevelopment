/*-----------------------------------------------
  FileName: init.c
  Author: Bingkan
  Date: 2013/12/24
  Version: 1.0
  Describe: ����ģ���ʼ������
------------------------------------------------*/
#include "init.h"

void Uart_Init(void)		//1200bps@12MHz
{
	SCON = 0x50;		//8λ����,�ɱ䲨����
	TMOD |= 0x20;		//�趨��ʱ��1Ϊ8λ�Զ���װ��ʽ
	TH1 = 0xE6;	    	//�趨��ʱ����װֵ
	TR1 = 1;	    	//������ʱ��1
	IPH |= 0x10;        //�ж����ȼ����
	PS = 1;             //�ж����ȼ����
	ES = 1;             //�򿪴����ж�
	TI = 0;
}

void Loop_Init(void)
{
	P2=0xfe;           //����ʼ��ֵ ����ɶ�������1111 1110
}

void Int_Init(void)
{
	EX0=1;  		//�����ⲿ�ж� 0
	IT0=0; 
	EX1=1;  		//�����ⲿ�ж� 1
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
