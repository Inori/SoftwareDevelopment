#include "func.h"


/************************LED***************************/

//LED…¡À∏
void flash_led(void)
{
  PTBD_PTBD0 =~ PTBD_PTBD0;
  PTBD_PTBD1 =~ PTBD_PTBD1;
  //delay(1000);
  wait(100);
}



/************************KeyBoard************************/

