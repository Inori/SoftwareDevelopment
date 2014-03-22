#ifndef TOOLS_H
#define TOOLS_H

#include "include.h"


                         
void delay(uint ms);

void wait(uint ms);

int abs(int x);

uchar average(uchar* data, uint len);

byte bcd2dec(byte bcd);

void sci_senddata(byte d);

void sci_sendchar(char c);

void sci_sendstr(char *s);

void delay_s(void);

void IIC_WRITE(byte DeviceAdd,byte RegAdd,byte Cnt,byte* p);

void IIC_READ(byte DeviceAdd,byte RegAdd,byte Cnt,byte* p);


#endif