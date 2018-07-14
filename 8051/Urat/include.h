#ifndef _INCLUDE_H_
#define _INCLUDE_H_

#include "STC89C52RC.h" 
#include "tools.h"
#include "init.h"
#include "uart.h"
#include "led.h"

#define IBUS 12000000             //总线时钟
#define T1MS (65536-IBUS/12/1000) //定时器周期 10ms


extern unsigned int result;
extern unsigned char const ledtbl[];
extern unsigned int disnum;







#endif
