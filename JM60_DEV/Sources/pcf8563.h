#ifndef PCF8563_H
#define PCF8563_H

#include "include.h"

#define PCF8563_W_Addr 0xA2


void PCF8563_Init(void);

void PCF8563_Read_Info(void);

#endif