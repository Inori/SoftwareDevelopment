#ifndef INCLUDE_H
#define INCLUDE_H

#include <hidef.h> 
#include "derivative.h"
#include <stdtypes.h>

#include "init.h"
#include "tools.h"
#include "func.h"
#include "ht1628.h"

#define BusClock 9200000

#define LED0 PTBD_PTBD0
#define LED1 PTBD_PTBD1

#define Key4 PTBD_PTBD4
#define Key5 PTBD_PTBD5

//Potentiometer
#define Pttm PTBD_PTBD2


extern ulong T;








#endif