#include "include.h" 

unsigned int SPEED = 0;
//舵机      
unsigned int CENTER = 0;
unsigned int LEFT = 0;
unsigned int RIGHT = 0;


float angle = 0.0;    //舵机转角占空比






void main(void)
{
  

  DisableInterrupts;
  
  Init_All();
  
  DLY_ms(1000);
  PACNT=0;        //将计数器清零
	EnableInterrupts;

  
  while(1)
  {

    CENTER = ATD0DR0;
    RIGHT  = ATD0DR1;
    LEFT   = ATD0DR2;

  } 
  
  
 
}
