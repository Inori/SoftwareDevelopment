/*-----------------------------------------------
  FileName: main.c
  Author: Bingkan
  Date: 2013/12/24
  Version: 1.0
  Describe: ȫ�ֵ���
------------------------------------------------*/

#include "include.h" 

//�������ݴ洢�����������յ����ݺ��Զ��洢1�ֽڵ��˱���
unsigned int result = 0;
//�������ʾ������
unsigned int disnum = 1234;

void main (void)
{
	EA    = 0;                  //�ر����ж�
	ALL_Init();
    EA    = 1;                  //�����ж�
    
	
	while (1)                   //��ѭ�������κζ���
    {
		//�������ʾ��������ʱ��ر�
		loopled();
    }
}



