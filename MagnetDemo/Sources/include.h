#ifndef INCLUDE_H
#define INCLUDE_H

#include <hidef.h>      
#include "derivative.h"
#include <stdio.h>


#include "init.h"
#include "OLED.h"

#include "debug.h"

#include "Delay.h"
#include "Speed_PID.h"

//所有宏定义
#define MID 1180
#define LEFT_DIE 1050
#define RIGHT_DIE 1310

#define KP1 300 //用于调整占空比 
#define KD1 50 //用于调整占空比 

#define KP2 230 //用于调整占空比 
#define KD2 20 //用于调整占空比 


#define STEER PWMDTY67
#define IO1 PWMDTY45  //正转
#define IO2 PWMDTY23  //反转

#define PID_Dead 5
#define Speed_Max 5000
#define SpeedMax 150


#endif

