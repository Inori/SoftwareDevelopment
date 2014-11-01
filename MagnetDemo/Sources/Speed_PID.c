/******************************************************************************\
*  Copyright (C) 2013 LI Nan, Tianjin University 
*  ALL RIGHTS RESERVED
*  Filename:Speed_PID.c
*  Describe: 与速度PID控制相关的所有函数
*  Version: 1.0
*  Author: LI Nan
\******************************************************************************/




#include "Speed_PID.h"


char v_Kp = 10;      
char v_Ki = 1;     
char v_Kd = 1;


int Motor_PWM;
int v_PID;         //电机控制 PWM 输出增量
unsigned int SpeedBack;   //电机速度反馈值
unsigned int SpeedWant = 50;
int SpeedPreError;  
int SpeedError;  
int d_SpeedError;
int d_PreSpeedError;
int dd_SpeedError;   



unsigned int abs(int b)
{
  if(b<0) return -b;
  else return b;
}


void SetSpeed(int PWM) 
{
  if(PWM == 0)
  { 
    IO1 = 0;
    IO2 = 0; 
  } 
  else if(PWM > 0)
  { 
    IO1 = abs(PWM);
    IO2 = 0; 
  } 
  else if(PWM < 0)
  { 
    IO1 = 0;
    IO2 = abs(PWM);  
  }
}

void v_PIDInit(void)
{ 
  SpeedBack = 0;
  SpeedPreError = 0;
  d_PreSpeedError = 0; 
  d_SpeedError = 0;
  dd_SpeedError = 0;
}     

int PID(unsigned int SpeedSet)
{
  SpeedError = SpeedSet-SpeedBack;
  d_SpeedError = SpeedError-SpeedPreError;
  dd_SpeedError = d_SpeedError-d_PreSpeedError;
  SpeedPreError = SpeedError;     
  d_PreSpeedError = d_SpeedError;
  //////////////////////////////////////////////////////////////////////////////////
  if(abs(SpeedError) < PID_Dead) return 0;
  else return (v_Kp*d_SpeedError+v_Ki*SpeedError+v_Kd*dd_SpeedError);
}

void Set_Speed(int v) 
{
  v_PID = PID(v);
  Motor_PWM = Motor_PWM+v_PID;
  if(Motor_PWM > Speed_Max)  Motor_PWM = Speed_Max;
  else if(Motor_PWM < -Speed_Max)  Motor_PWM = -Speed_Max;  
  /////////////////////////////////////////////////////////////////////////////
  SetSpeed(Motor_PWM); 
}
