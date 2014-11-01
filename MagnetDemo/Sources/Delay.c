/******************************************************************************\
*  Copyright (C) 2011 Nan Li, Tianjin University 
*  ALL RIGHTS RESERVED
*  Filename:Delay.c
*  Version: 1.0
*  Author: Nan Li
\******************************************************************************/




void DLY_ms(int ms)
{
   int ii,jj;
   if (ms<1) ms=1;
     for(ii=0;ii<ms;ii++)
     for(jj=0;jj<14000;jj++);   //80MHz--1ms
} 

void DLY_us(int us)
{
   int ii,jj;
   if (us<1) us=1;
     for(ii=0;ii<us;ii++)
     for(jj=0;jj<20;jj++);   //80MHz--1us
} 