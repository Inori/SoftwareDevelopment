#include "ht1628.h"


//SEG Code
const byte LEDseg[11]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0};

void LED_CMD_SEND(byte data)
{
  byte i;
  STB = 1;
  __asm nop;
  STB = 0;
  __asm nop;
  
  SPIBUF = data;
  while(!SPI1S_SPTEF);
  for(i=0;i<10;i++);
}

void LED_DATA_SEND(byte data)
{
  byte i;
  SPIBUF = data;
  
  while(!SPI1S_SPTEF);
  for(i=0;i<10;i++);   
}

void seg_display(uint disnum)
{
  byte dis1=0, dis2=0, dis3=0, dis4=0;

  dis1 = disnum / 1000;
  dis2 = (disnum % 1000) / 100;
  dis3 = (disnum % 100) / 10;
  dis4 = disnum % 10;
  
  //初始化
  LED_CMD_SEND(0x00);   // 4位13段
  LED_CMD_SEND(0x40);   // 地址自动增加模式
  LED_CMD_SEND(0xC0);   // 设置显示地址为00

  LED_DATA_SEND(LEDseg[dis1]);
  LED_DATA_SEND(LEDseg[10]); //dummy
  
  LED_DATA_SEND(LEDseg[dis2]);
  LED_DATA_SEND(LEDseg[10]); //dummy
  
  LED_DATA_SEND(LEDseg[dis3]);
  LED_DATA_SEND(LEDseg[10]); //dummy
  
  LED_DATA_SEND(LEDseg[dis4]);
  LED_DATA_SEND(LEDseg[10]); //dummy
  
  LED_CMD_SEND(0x8B);   
  }