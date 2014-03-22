#include "include.h"

//È«¾ÖÊ±¼ä±äÁ¿,Ã¿10ms¼Ó1
ulong T = 0;

//ÊıÂë¹ÜÏÔÊ¾ÉèÖÃ±êÖ¾
//0 ÏÔÊ¾Ê±ÖÓĞ¾Æ¬ÃëÊı
//1 ÏÔÊ¾SCI½ÓÊÕµ½µÄÊı¾İ
//Í¬Ê±°´ÏÂkey4 key5¸Ä±ä¸ÃÎ»
//byte g_segflag = 0;

void main(void) 
{

  byte test[5] = {1,2,3,4,5};
  byte res[5] = {0};

  DisableInterrupts;
  ALL_Init();
  EnableInterrupts; 

  //seg_display(1234);
  
  PCF8563_Init();
  
  HCS08_EE_Init();//³õÊ¼»¯Flash¿ØÖÆ¼Ä´æÆ÷¡
  HCS08_EE_WriteData(test, 5);
  HCS08_EE_ReadData(res, 5);
    


  while(TRUE)
  {  
     //sci_sendstr("What the fuck!\r\n");
     //sci_senddata(0x20);
     //flash_led();
     PCF8563_Read_Info();
     seg_display(100*(uint)bcd2dec(Minutes)+(uint)bcd2dec(Seconds));
  }

}
