#include "include.h"

//上位机可接受命令：
//"ec"  eye close
//"eo"  eye open
//"mc"  mouse close
//"mo"  mouse open
//"\xFF\x45" 调整窗口透明度，后面的 "\x45" 为任意alpha值(0 - 255)

//用来计算AD平均值的缓冲数组
uchar ad_data[10] = {0};

void interrupt VectorNumber_Vkeyboard KBI()
{
  static  Bool flag4 = FALSE;
  static  Bool flag5 = FALSE;
  
  delay(10);
  if(Key4 == 0)
  {
     if(flag4)
     {
       sci_sendstr("ec");
       flag4 = FALSE;
     } 
     else
     {
       sci_sendstr("eo");
       flag4 = TRUE;
     } 
  }
  
  if(Key5 == 0)
  {
     if(flag5)
     {
       sci_sendstr("mo");
       flag5 = FALSE;
     } 
     else
     {
       sci_sendstr("mc");
       flag5 = TRUE;
     } 
  }
  
  KBISC_KBACK = 1; //clean flag
}


void interrupt VectorNumber_Vrtc RTC()
{
  T += 1;
  RTCSC_RTIF = 1; //clean flag
}

//定义放到AD中断函数里就出错，为什么？？
uchar ad_avr = 0; 
uchar pre_ad_avr = 0;
  
void interrupt VectorNumber_Vadc ATD()
{
  byte i = 0;

  //队列前移一个
  for(i=0; i<9; i++)
  {
    ad_data[i] = ad_data[i+1];
  }
  ad_data[9] = ADCR;
  
  ad_avr = average(ad_data, 10);
  
  //怎么能让单片机不转电位器就不发数？？？
  if(ad_avr != pre_ad_avr)
  {
     //sci_senddata(0xFF);
     //sci_senddata(ad_avr);
     //seg_display(ad_avr);
  }
  pre_ad_avr = ad_avr;   
}

void interrupt VectorNumber_Vsci1rx SCI_Receive()
{
   uint dummy = 0;
   uint val = 0;
   
   dummy = SCI1S1; //clear flag
   
   SCI1C2_RIE = 0; //disable receive interrupt
   
   val = SCI1D;
   while(!SCI1S1_RDRF);
   val = val * 0x100 + SCI1D ;
   seg_display(val);
   
   SCI1C2_RIE = 1; //enable receive interrupt
   
}
/*
void interrupt VectorNumber_Vtpm1ch0 PWM0()
{
  byte dummy = 0;
  
  if(LED1DCR < LEDMDR)
     LED1DCR += STEP;
  else
     LED1DCR -= STEP;
 
  dummy = TPM1C0SC;  //clear flag
  TPM1C0SC_CH0F = 0; //clear flag
}
*/


//为什么不能一个亮一个灭？？？
void interrupt VectorNumber_Vtpm1ovf TPM()
{
  static byte flag1 = 1;
  static byte flag2 = 1;
  byte dummy = 0;
   
  if(flag1)
  {
    LED1DCR += STEP;
    if(LED1DCR == LEDMDR)
      flag1 = 0;
  }
  else
  {
    LED1DCR -= STEP;
    if(LED1DCR == 0)
      flag1 = 1;
  }
  
  if(flag2)
  {
    LED2DCR -= STEP;
    if(LED2DCR == 0)
      flag2 = 0;
  }
  else
  {
    LED2DCR += STEP;
    if(LED2DCR == LEDMDR)
      flag2 = 1;
  }
  
  dummy = TPM1SC;
  TPM1SC_TOF = 0;//clear flag
}





