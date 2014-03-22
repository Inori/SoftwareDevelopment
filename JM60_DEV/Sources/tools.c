#include "tools.h"

/************************Delay Func***************************/
void delay(uint ms)
{
  uint i=0, j=0;
  for(i=0;i<ms;i++)
  {
    for(j=0;j<370;j++)
    {
      __asm nop;
    }
  }
}

//精确延时，单位10ms，需开启RTC
void wait(uint ten_ms)
{
   ulong temp = T;
   while(T - temp <= ten_ms);
}
/************************Math Func************************/
int abs(int x)
{
  if(x < 0)
    return (0-x);
  else
    return x;
}

byte average(byte* data, uint len)
{
  uint i = 0;
  uint sum = 0;
  for(i=0; i<len; i++)
  {
     sum += data[i];
  }
  return sum/len;
}

byte bcd2dec(byte bcd)
{
  byte t = 0;//十位
  byte u = 0;//个位
  
  u = bcd & 0x0F;
  t = bcd >> 4;
  return (t * 10 + u);
}

/************************SCI***************************/
void sci_senddata(byte d)
{
   SCI1D = d;
   while(!SCI1S1_TDRE);
}

void sci_sendchar(char c)
{
   SCI1D = c;
   while(!SCI1S1_TDRE);
}

void sci_sendstr(char *s)
{
  while(*s)
  {
    sci_sendchar(*s++);
  }
}

/************************IIC***************************/
                                                
void delay_s(void)
{               //每两个数据读取之间需要间隔一段时间
  uchar i;
  for(i=0;i<10;i++);
}

void IIC_WRITE(byte DeviceAdd,byte RegAdd,byte Cnt,byte* p)
{
    byte temp,i;
   
    temp = IICS;       //清除IIC中断标志位                                          
    IICS_IICIF = 1;
    
    IICC1_MST=0;       
    IICC1_TX=1;         //IIC发送数据使能
    IICC1_MST=1;        //设为主模式，MST由0变成1产生S信号
         
    IICD=DeviceAdd;     //发送芯片地址，写命令
    while(!IICS_IICIF );
    IICS_IICIF=1;        //等待发送完成，清中断
    while(IICS_RXAK);    //等待从器件响应ACK
    
    IICD=RegAdd;       //发送寄存器地址
    while(!IICS_IICIF);
    IICS_IICIF=1;        //等待发送完成，清中断
    while(IICS_RXAK);   //等待从器件响应ACK
    
    for(i=0;i<Cnt;i++) 
    {   //连续发送N字节数据
      IICD = p[i];
      while(!IICS_IICIF);
      IICS_IICIF=1;
      while(IICS_RXAK);  //等待从器件响应ACK，连续写N个字节，每次都要应答    
    }
    
    IICC1_MST=0;      //MST由1变为0，产生P信号  
    delay_s();
}

void IIC_READ(byte DeviceAdd,byte RegAdd,byte Cnt,byte* p)
{
    byte i,temp;
    IICC1_RSTA=0;     //将重复开始信号复位
     
    temp = IICS;       //将中断信号清除
    IICS_IICIF = 1;
    
    IICC1_MST=0;
    IICC1_TX=1;       //写使能
    IICC1_MST=1;      //置为主模式，产生S信号
    
    IICD=DeviceAdd ;   //发送芯片地址，写命令
    while(!IICS_IICIF);
    IICS_IICIF=1;       //等待发送完成，清中断
    while(IICS_RXAK);   //等待从器件响应ACK		
    
    IICD=RegAdd;        //发送寄存器地址
    while(!IICS_IICIF);     //等待发送完成，清中断
    IICS_IICIF=1;
    while(IICS_RXAK);     //等待从器件响应ACK

    if(Cnt>1) IICC1_TXAK = 0;      //如果下面只发送一个字节，将发送NACK信号
    else      IICC1_TXAK = 1; 	  //如果下面连续发送几个字节，发送ACK信号
    
    IICC1_RSTA=1;         //产生重复开始信号
    
    IICD = DeviceAdd+1;   //发送芯片地址,读命令   
    while(!IICS_IICIF);      //等待发送完成，清中断
    IICS_IICIF=1;
    while(IICS_RXAK);     //等待从器件响应ACK
    
    temp=IICD;    //在设置主设备读使能时，需要先读IICD，否则会产生不必要的错误
    IICC1_TX=0;    //读使能

    if(Cnt>1)
    {
        for(i=0;i<Cnt-1;i++)
        {
          while(!IICS_IICIF); //是否接收到数据
          IICS_IICIF=1;
          p[i] = IICD;        //保存数据
        }
        IICC1_TXAK = 1;     //最后一个字节，发送NACK
    }
    while(!IICS_IICIF);      //是否接收到数据
    IICS_IICIF=1;
    IICC1_MST=0;         //发送停止信号
    p[Cnt-1] = IICD;        //保存数据
}

                          

