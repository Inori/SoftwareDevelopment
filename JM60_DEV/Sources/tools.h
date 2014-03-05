#ifndef TOOLS_H
#define TOOLS_H

#include "include.h"

void delay(uint ms);

void wait(uint ms);

int abs(int x);

uchar average(uchar* data, uint len);

void sci_senddata(byte d);

void sci_sendchar(char c);

void sci_sendstr(char *s);



#endif