#include "tools.h"

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

int abs(int x)
{
  if(x < 0)
    return (0-x);
  else
    return x;
}

uchar average(uchar* data, uint len)
{
  uint i = 0;
  uint sum = 0;
  for(i=0; i<len; i++)
  {
     sum += data[i];
  }
  return sum/len;
}

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
