/******************************************************************************\
*  Copyright (C) 2013 LI Nan, Tianjin University 
*  ALL RIGHTS RESERVED
*  Filename:Speed_PID.h
*  Version: 1.0
*  Author: LI Nan
\******************************************************************************/


#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
//#include "define.h"
#include "include.h" 





unsigned int abs(int b);
void v_PIDInit(void);
void SetSpeed(int PWM);
void Set_Speed(int v);



