#ifndef INCLUDE_H
#define INCLUDE_H

#include <hidef.h> 
#include "derivative.h"
#include <stdtypes.h>

#include "init.h"
#include "tools.h"
#include "func.h"
#include "ht1628.h"
#include "pcf8563.h"
#include "HCS08_EEPROM_API.h"

#define BusClock 9200000

#define LED1 PTBD_PTBD0
#define LED2 PTBD_PTBD1

#define Key4 PTBD_PTBD4
#define Key5 PTBD_PTBD5

//Potentiometer
#define Pttm PTBD_PTBD2

#define STEP 200
#define LEDMDR TPM1MOD //LED Modulo Register
#define LED1DCR TPM1C0V //LED1(PTED2) Duty Cycle Register
#define LED2DCR TPM1C1V //LED2(PTED3) Duty Cycle Register


extern ulong T;
//extern byte g_segflag;

extern byte Seconds;
extern byte Minutes; 
extern byte Hours;
extern byte Days; 
extern byte Weekdays; 
extern byte Months; 
extern byte Years;








#endif