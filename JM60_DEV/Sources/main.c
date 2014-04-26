#include "include.h"

//全局时间变量,每10ms加1
ulong T = 0;
//屏幕显示字符串缓冲
char buff[20]= {0};

//数码管显示设置标志
//0 显示时钟芯片秒数
//1 显示SCI接收到的数据
//同时按下key4 key5改变该位
//byte g_segflag = 0;

void main(void) 
{
  byte segnum = 0;
  int DispCnt = 0;
  

  DisableInterrupts;
  ALL_Init();
  EnableInterrupts; 

  
   /*
  
  //HCS08_EE_WriteRecord(&segnum);  //Init segnum
  HCS08_EE_ReadRecord(&segnum);
  seg_display(segnum);
  
  sprintf(buff, "The %dth time on", segnum); 
  LCD_CLS_ALL();   //清屏
  LCD_Print(0,0,buff);        //将整个数据写在液晶第一行
  
  segnum++;
  HCS08_EE_WriteRecord(&segnum);
  
  wait(200);
   */
  while(TRUE)
  { 
  
  
    //sci_sendstr("What the fuck!\r\n");
    //sci_senddata(0x20);
    
    //flash_led();
    
    PCF8563_Read_Info();
    //seg_display((uint)bcd2dec(Seconds));
    
    sprintf(buff, "%02d:%02d", (uint)bcd2dec(Minutes),(uint)bcd2dec(Seconds)); 
    LCD_CLS_ALL();   //清屏语句
    LCD_Print(0,0,buff);        //将整个数据写在液晶第一行
    /*
    LCD_Print(1,0,Text+(DispCnt%16));     //将数据第二行显示利用for循环设//置滚动的效果
    DispCnt++;
    
    delay(500); */
  }

}
