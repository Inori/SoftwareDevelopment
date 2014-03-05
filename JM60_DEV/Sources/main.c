#include "include.h"

//全局时间变量,每10ms加1
ulong T = 0;

void main(void) 
{

  DisableInterrupts;
  ALL_Init();
  EnableInterrupts; 

  //seg_display(1234);
  while(TRUE)
  {  
     //sci_sendstr("What the fuck!\r\n");
     //sci_senddata(0x20);
     //flash_led();
  }

}
