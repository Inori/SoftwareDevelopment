#include "pcf8563.h"

byte Seconds, Minutes, Hours;
byte Days, Weekdays, Months, Years;
byte PCF8563_REG_READ[16]={0};  //对芯片读取寄存器值存放位置
byte PCF8563_REG_INIT[16]={ 
                            0x00,0x00, 
                            0x00,0x00,0x12,
                            0x17,0x05,0x02,0x12,
                            0x80,0x80,0x81,0x80,
                            0x03,0x03,0xff 
                          };   //对芯片初始化设置的数值


void PCF8563_Init(void)
{
  //Init PCF8563
  IIC_WRITE(PCF8563_W_Addr,0x00,16,PCF8563_REG_INIT); 
}


void PCF8563_Read_Info(void)
{
    IIC_READ(PCF8563_W_Addr,0x00,16,PCF8563_REG_READ);
    Seconds  = PCF8563_REG_READ[2];
    Minutes  = PCF8563_REG_READ[3];
    Hours    = PCF8563_REG_READ[4];
    Days     = PCF8563_REG_READ[5];
    Weekdays = PCF8563_REG_READ[6];
    Months   = PCF8563_REG_READ[7];  
    Years    = PCF8563_REG_READ[8];
}