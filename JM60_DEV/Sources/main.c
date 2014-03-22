#include "include.h"

//全局时间变量,每10ms加1
ulong T = 0;

//数码管显示设置标志
//0 显示时钟芯片秒数
//1 显示SCI接收到的数据
//同时按下key4 key5改变该位
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
  
  HCS08_EE_Init();//初始化Flash控制寄存器�
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
