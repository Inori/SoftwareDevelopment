#ifndef INIT_H
#define INIT_H

#include "include.h"

void MCU_Init(void);

void MCG_Init(void);
 
void LED_Init(void);

void KBI_Init(void);

void RTC_Init(void);

void PTED_Init(void);

void SPI_Init(void);

void ATD_Init(void);

void SCI_Init(void);

void PWM_Init(void);

void IIC_Init(void);

void ALL_Init(void);

#endif