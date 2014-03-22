#include "func.h"


/************************LED***************************/

//LED…¡À∏
void flash_led(void)
{
  LED1 =~ LED1;
  LED2 =~ LED2;
  //delay(1000);
  wait(100);
}






