#include "include.h"




extern CENTER;
extern LEFT;
extern RIGHT;
extern SPEED;
extern angle;
extern SpeedBack;   //左电机速度反馈值
extern SpeedWant;

char str[20]="";
float Error = 0;
float PreError = 0;
float dError = 0;
unsigned int T = 0;

float GetSpecific(int left, int right)

{
  float d = left - right;
  float arv = (left+right)/2;
  return (d/arv)/2; 
}



#pragma CODE_SEG __NEAR_SEG NON_BANKED



//获取并计算速度，采样时间有待调试
void interrupt VectorNumber_Vpit0 GetSpeed()

{
    unsigned int steer_temp;
    
    T++;
    
    SpeedBack = PACNT;
    PACNT = 0;
    
    LCD_CLS();
    sprintf(str, "Speed=%d", SpeedBack);
    LCD_P6x8Str(0, 1, str);
    /*
    sprintf(str, "Certer=%4d", CENTER);
    LCD_P6x8Str(0, 0, str);
    
    sprintf(str, "Left=%4d", LEFT);
    LCD_P6x8Str(0, 1, str);
    
    sprintf(str, "Right=%4d", RIGHT);
    LCD_P6x8Str(0, 2, str);
    
    sprintf(str, "Specific=%4d",(int)(K*GetSpecific(LEFT, RIGHT)));
    LCD_P6x8Str(0, 3, str); 
    */
    PreError = Error;
    Error = GetSpecific(LEFT, RIGHT);
    dError = Error - PreError;
    
    
    
    switch (Error>0)
    {
      case 1:steer_temp = MID - (int)(KP1*Error*Error+KD1*dError);  break;
      
      
      case 0:steer_temp = MID + (int)(KP2*Error*Error+KD2*dError);  break;
    }
    if(steer_temp > RIGHT_DIE) steer_temp = RIGHT_DIE;
    else if(steer_temp < LEFT_DIE) steer_temp = LEFT_DIE;
    
    STEER = steer_temp;
    //SpeedWant = (int)(Error * SpeedMax) + 200;
    Set_Speed(220); 
    if(T>50)
    {
      if(SpeedBack < 20) 
      {
        SetSpeed(0);
        for(;;);
      }  
    }
    
    
    PITTF_PTF0 = 1;//清标志位
}



#pragma CODE_SEG DEFAULT