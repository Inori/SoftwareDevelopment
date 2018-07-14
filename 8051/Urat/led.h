#ifndef _LED_H_
#define _LED_H_

#include "include.h"


sbit LED1 = P1^3;
sbit LED2 = P1^2;
sbit LED3 = P1^1;
sbit LED4 = P1^0;

void display(unsigned int num);




#endif
