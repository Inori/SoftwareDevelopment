#include "derivative.h" 
#include "OLED.h"

#define LCD_SCL PORTB_PB4  
#define LCD_SDA	PORTB_PB3
#define LCD_RST PORTB_PB2 
#define LCD_DC  PORTB_PB1 

#define XLevelL		0x00
#define XLevelH		0x10
#define XLevel		((XLevelH&0x0F)*16+XLevelL)
#define Max_Column	128
#define Max_Row		  64
#define	Brightness	0xCF 
#define X_WIDTH 128
#define Y_WIDTH 64
//======================================
const byte F6x8[][6] =
{
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // sp
    { 0x00, 0x00, 0x00, 0x2f, 0x00, 0x00 },   // !
    { 0x00, 0x00, 0x07, 0x00, 0x07, 0x00 },   // "
    { 0x00, 0x14, 0x7f, 0x14, 0x7f, 0x14 },   // #
    { 0x00, 0x24, 0x2a, 0x7f, 0x2a, 0x12 },   // $
    { 0x00, 0x62, 0x64, 0x08, 0x13, 0x23 },   // %
    { 0x00, 0x36, 0x49, 0x55, 0x22, 0x50 },   // &
    { 0x00, 0x00, 0x05, 0x03, 0x00, 0x00 },   // '
    { 0x00, 0x00, 0x1c, 0x22, 0x41, 0x00 },   // (
    { 0x00, 0x00, 0x41, 0x22, 0x1c, 0x00 },   // )
    { 0x00, 0x14, 0x08, 0x3E, 0x08, 0x14 },   // *
    { 0x00, 0x08, 0x08, 0x3E, 0x08, 0x08 },   // +
    { 0x00, 0x00, 0x00, 0xA0, 0x60, 0x00 },   // ,
    { 0x00, 0x08, 0x08, 0x08, 0x08, 0x08 },   // -
    { 0x00, 0x00, 0x60, 0x60, 0x00, 0x00 },   // .
    { 0x00, 0x20, 0x10, 0x08, 0x04, 0x02 },   // /
    { 0x00, 0x3E, 0x51, 0x49, 0x45, 0x3E },   // 0
    { 0x00, 0x00, 0x42, 0x7F, 0x40, 0x00 },   // 1
    { 0x00, 0x42, 0x61, 0x51, 0x49, 0x46 },   // 2
    { 0x00, 0x21, 0x41, 0x45, 0x4B, 0x31 },   // 3
    { 0x00, 0x18, 0x14, 0x12, 0x7F, 0x10 },   // 4
    { 0x00, 0x27, 0x45, 0x45, 0x45, 0x39 },   // 5
    { 0x00, 0x3C, 0x4A, 0x49, 0x49, 0x30 },   // 6
    { 0x00, 0x01, 0x71, 0x09, 0x05, 0x03 },   // 7
    { 0x00, 0x36, 0x49, 0x49, 0x49, 0x36 },   // 8
    { 0x00, 0x06, 0x49, 0x49, 0x29, 0x1E },   // 9
    { 0x00, 0x00, 0x36, 0x36, 0x00, 0x00 },   // :
    { 0x00, 0x00, 0x56, 0x36, 0x00, 0x00 },   // ;
    { 0x00, 0x08, 0x14, 0x22, 0x41, 0x00 },   // <
    { 0x00, 0x14, 0x14, 0x14, 0x14, 0x14 },   // =
    { 0x00, 0x00, 0x41, 0x22, 0x14, 0x08 },   // >
    { 0x00, 0x02, 0x01, 0x51, 0x09, 0x06 },   // ?
    { 0x00, 0x32, 0x49, 0x59, 0x51, 0x3E },   // @
    { 0x00, 0x7C, 0x12, 0x11, 0x12, 0x7C },   // A
    { 0x00, 0x7F, 0x49, 0x49, 0x49, 0x36 },   // B
    { 0x00, 0x3E, 0x41, 0x41, 0x41, 0x22 },   // C
    { 0x00, 0x7F, 0x41, 0x41, 0x22, 0x1C },   // D
    { 0x00, 0x7F, 0x49, 0x49, 0x49, 0x41 },   // E
    { 0x00, 0x7F, 0x09, 0x09, 0x09, 0x01 },   // F
    { 0x00, 0x3E, 0x41, 0x49, 0x49, 0x7A },   // G
    { 0x00, 0x7F, 0x08, 0x08, 0x08, 0x7F },   // H
    { 0x00, 0x00, 0x41, 0x7F, 0x41, 0x00 },   // I
    { 0x00, 0x20, 0x40, 0x41, 0x3F, 0x01 },   // J
    { 0x00, 0x7F, 0x08, 0x14, 0x22, 0x41 },   // K
    { 0x00, 0x7F, 0x40, 0x40, 0x40, 0x40 },   // L
    { 0x00, 0x7F, 0x02, 0x0C, 0x02, 0x7F },   // M
    { 0x00, 0x7F, 0x04, 0x08, 0x10, 0x7F },   // N
    { 0x00, 0x3E, 0x41, 0x41, 0x41, 0x3E },   // O
    { 0x00, 0x7F, 0x09, 0x09, 0x09, 0x06 },   // P
    { 0x00, 0x3E, 0x41, 0x51, 0x21, 0x5E },   // Q
    { 0x00, 0x7F, 0x09, 0x19, 0x29, 0x46 },   // R
    { 0x00, 0x46, 0x49, 0x49, 0x49, 0x31 },   // S
    { 0x00, 0x01, 0x01, 0x7F, 0x01, 0x01 },   // T
    { 0x00, 0x3F, 0x40, 0x40, 0x40, 0x3F },   // U
    { 0x00, 0x1F, 0x20, 0x40, 0x20, 0x1F },   // V
    { 0x00, 0x3F, 0x40, 0x38, 0x40, 0x3F },   // W
    { 0x00, 0x63, 0x14, 0x08, 0x14, 0x63 },   // X
    { 0x00, 0x07, 0x08, 0x70, 0x08, 0x07 },   // Y
    { 0x00, 0x61, 0x51, 0x49, 0x45, 0x43 },   // Z
    { 0x00, 0x00, 0x7F, 0x41, 0x41, 0x00 },   // [
    { 0x00, 0x55, 0x2A, 0x55, 0x2A, 0x55 },   // 55
    { 0x00, 0x00, 0x41, 0x41, 0x7F, 0x00 },   // ]
    { 0x00, 0x04, 0x02, 0x01, 0x02, 0x04 },   // ^
    { 0x00, 0x40, 0x40, 0x40, 0x40, 0x40 },   // _
    { 0x00, 0x00, 0x01, 0x02, 0x04, 0x00 },   // '
    { 0x00, 0x20, 0x54, 0x54, 0x54, 0x78 },   // a
    { 0x00, 0x7F, 0x48, 0x44, 0x44, 0x38 },   // b
    { 0x00, 0x38, 0x44, 0x44, 0x44, 0x20 },   // c
    { 0x00, 0x38, 0x44, 0x44, 0x48, 0x7F },   // d
    { 0x00, 0x38, 0x54, 0x54, 0x54, 0x18 },   // e
    { 0x00, 0x08, 0x7E, 0x09, 0x01, 0x02 },   // f
    { 0x00, 0x18, 0xA4, 0xA4, 0xA4, 0x7C },   // g
    { 0x00, 0x7F, 0x08, 0x04, 0x04, 0x78 },   // h
    { 0x00, 0x00, 0x44, 0x7D, 0x40, 0x00 },   // i
    { 0x00, 0x40, 0x80, 0x84, 0x7D, 0x00 },   // j
    { 0x00, 0x7F, 0x10, 0x28, 0x44, 0x00 },   // k
    { 0x00, 0x00, 0x41, 0x7F, 0x40, 0x00 },   // l
    { 0x00, 0x7C, 0x04, 0x18, 0x04, 0x78 },   // m
    { 0x00, 0x7C, 0x08, 0x04, 0x04, 0x78 },   // n
    { 0x00, 0x38, 0x44, 0x44, 0x44, 0x38 },   // o
    { 0x00, 0xFC, 0x24, 0x24, 0x24, 0x18 },   // p
    { 0x00, 0x18, 0x24, 0x24, 0x18, 0xFC },   // q
    { 0x00, 0x7C, 0x08, 0x04, 0x04, 0x08 },   // r
    { 0x00, 0x48, 0x54, 0x54, 0x54, 0x20 },   // s
    { 0x00, 0x04, 0x3F, 0x44, 0x40, 0x20 },   // t
    { 0x00, 0x3C, 0x40, 0x40, 0x20, 0x7C },   // u
    { 0x00, 0x1C, 0x20, 0x40, 0x20, 0x1C },   // v
    { 0x00, 0x3C, 0x40, 0x30, 0x40, 0x3C },   // w
    { 0x00, 0x44, 0x28, 0x10, 0x28, 0x44 },   // x
    { 0x00, 0x1C, 0xA0, 0xA0, 0xA0, 0x7C },   // y
    { 0x00, 0x44, 0x64, 0x54, 0x4C, 0x44 },   // z
    { 0x14, 0x14, 0x14, 0x14, 0x14, 0x14 }    // horiz lines
};


//数据水平，字节垂直
byte longqiu96x64[768] = {
/* 
  0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
  0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X80,0X80,0X80,0XC0,0XC0,0XC0,
  0XC0,0XC0,0X60,0X60,0X60,0X60,0X60,0X70,0X70,0X70,0X30,0X30,0X30,0X30,0X30,0X30,
  0X30,0X30,0X30,0X30,0X30,0X30,0X30,0X70,0X70,0X60,0X60,0X60,0X60,0X60,0XE0,0XC0,
  0XC0,0XC0,0XC0,0X80,0X80,0X80,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
  0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
  0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X80,0X80,0XC0,0XE0,0X60,0X30,
  0X38,0X18,0X1C,0X0C,0X0E,0X06,0X06,0X03,0X03,0X03,0X01,0X01,0X01,0X01,0X00,0X00,
  0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
  0X80,0XC0,0XC0,0X80,0X00,0X00,0X00,0X00,0X00,0X80,0XC0,0XE0,0XE0,0XE0,0X60,0X60,
  0XE0,0XE0,0XC0,0X81,0X01,0X01,0X03,0X03,0X03,0X07,0X06,0X0E,0X0C,0X1C,0X18,0X38,
  0X30,0X70,0X60,0XC0,0XC0,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
  0X00,0X00,0X00,0X80,0XC0,0XF0,0X38,0X9C,0X8E,0XC7,0XC3,0XE1,0XF1,0XB0,0X00,0X00,
  0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XC0,0XE0,0XF0,0X70,0X78,0XF0,0XF0,0XE0,
  0X80,0X30,0XF8,0XF8,0XF8,0XF8,0X1C,0X3C,0XFC,0XF8,0XF0,0X00,0XF0,0XF8,0XFC,0XFC,
  0XCF,0XCF,0XFF,0XFF,0XFC,0XF8,0X70,0XFC,0XFF,0XFF,0XFF,0X0F,0X01,0X70,0X70,0X30,
  0X00,0XFF,0XFF,0XFF,0XFF,0XFC,0X00,0XE0,0XE0,0XEE,0XEE,0X0E,0X00,0XC0,0XC0,0X80,
  0X80,0X00,0X00,0X00,0X01,0X03,0X03,0X06,0X0C,0X3C,0X70,0XE0,0XC0,0X00,0X00,0X00,
  0X00,0XF0,0XFE,0X1F,0X03,0X00,0X00,0X01,0X01,0X01,0X03,0X0F,0X1F,0X7F,0XFE,0XFC,
  0XF0,0XE0,0X80,0X80,0X86,0XFE,0XFC,0XF0,0X87,0X1F,0X1E,0X78,0XF8,0XF8,0X7D,0X7F,
  0X1F,0X20,0X3F,0X3F,0X3F,0X3F,0X30,0X00,0X3F,0X3F,0X3F,0X7C,0XF0,0XF9,0XFF,0XFF,
  0XFF,0XCF,0X9F,0XFF,0XFB,0XF8,0X70,0X03,0X0F,0X1F,0X1F,0X3E,0X3C,0X36,0X76,0X7E,
  0X7E,0XFF,0XFF,0XEF,0X47,0X61,0XFC,0XFF,0XFF,0XCF,0XE3,0XF8,0XFE,0XFF,0X9F,0X87,
  0XE3,0XF0,0XFE,0XFC,0X3C,0X0C,0X00,0X00,0X00,0X00,0X00,0X01,0X07,0X7F,0XFC,0X80,
  0X00,0X1F,0XFF,0XF0,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X03,
  0X1F,0X1F,0X0F,0X0F,0X07,0X03,0X03,0X01,0X00,0X00,0X00,0X00,0X00,0X00,0XC0,0XC0,
  0X00,0X00,0X00,0XC0,0XC0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X01,0X03,0X03,
  0X03,0X03,0X03,0X01,0X01,0X00,0X00,0X80,0X00,0X00,0X00,0X00,0X00,0X80,0X80,0XC0,
  0XC0,0X80,0X01,0X01,0X00,0X00,0X00,0X00,0X00,0X00,0X01,0X03,0X03,0X03,0X07,0X0F,
  0X0F,0X0F,0X01,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XC0,0XFE,0X7F,0X03,
  0X00,0X00,0X00,0X03,0X07,0X1E,0X38,0X70,0XE0,0XC0,0X80,0X00,0X00,0X00,0X00,0X00,
  0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X06,0X86,0X82,0XC2,0X62,0X3A,0X8F,0X43,
  0XFA,0XEE,0X32,0X3A,0X1A,0X02,0X02,0X02,0X00,0X00,0X00,0X00,0X00,0X1C,0X1C,0X1C,
  0X00,0X00,0X00,0X00,0X00,0X00,0X03,0XFF,0XFF,0X09,0X09,0X09,0X09,0XF9,0XF9,0X09,
  0X08,0X08,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
  0X00,0X00,0X00,0X00,0X00,0X80,0XC0,0XC0,0X60,0X38,0X1C,0X0F,0X07,0X01,0X00,0X00,
  0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X01,0X01,0X03,0X07,0X0E,0X0C,0X18,
  0X18,0X30,0X30,0X60,0X60,0XC0,0XC2,0XC3,0X83,0X83,0X81,0X00,0X00,0X00,0X00,0X00,
  0X01,0X03,0X03,0X07,0X07,0X06,0X0E,0X06,0X07,0X03,0X00,0X00,0X00,0X00,0X00,0X00,
  0X00,0X00,0X03,0X03,0X03,0X03,0X03,0X03,0X03,0X03,0X03,0X03,0X03,0X03,0X03,0X03,
  0X03,0X07,0X07,0X07,0X06,0X06,0X80,0X80,0X80,0XC0,0XC0,0XE0,0X60,0X70,0X30,0X38,
  0X18,0X1C,0X0E,0X06,0X07,0X03,0X01,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
  0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
  0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X01,0X01,0X01,0X03,0X03,0X03,0X07,0X06,0X06,
  0X06,0X06,0X0E,0X0C,0X0C,0X0C,0X0C,0X0C,0X0C,0X1C,0X1C,0X1C,0X18,0X18,0X18,0X18,
  0X18,0X18,0X18,0X18,0X1C,0X1C,0X1C,0X0C,0X0C,0X0C,0X0C,0X0C,0X0C,0X0C,0X06,0X06,
  0X06,0X06,0X06,0X03,0X03,0X03,0X01,0X01,0X01,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
  0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
*/
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X80,0X80,0XC0,0XC0,0XC0,0X60,0X60,0X60,0X30,0X30,0X30,0X38,0X18,0X18,
0X18,0X18,0X0C,0X0C,0X0C,0X0C,0X0C,0X0E,0X0E,0X0E,0X06,0X06,0X06,0X06,0X06,0X06,
0X06,0X06,0X06,0X06,0X06,0X06,0X06,0X0E,0X0E,0X0C,0X0C,0X0C,0X0C,0X0C,0X1C,0X18,
0X18,0X18,0X18,0X30,0X30,0X30,0X70,0X60,0X60,0XE0,0XC0,0XC0,0X80,0X80,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X80,0XC0,0XE0,0X70,0X30,0X38,0X1C,0X0C,0X06,
0X07,0X03,0X03,0X01,0X01,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X80,0X80,0X80,0X00,0X00,0X00,0X00,0X00,0X80,0X80,
0XF0,0XF8,0XF8,0XF0,0X80,0X00,0X00,0X80,0XE0,0XF0,0XF8,0XFC,0X3C,0X1C,0X0C,0X0C,
0X1C,0XFC,0XF8,0XF0,0XE0,0X80,0X00,0X00,0X00,0XC0,0XC0,0XC1,0X01,0X03,0X03,0X07,
0X06,0X0E,0X0C,0X18,0X38,0X70,0X60,0XC0,0X80,0X80,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0XC0,0XF0,0X78,0X1E,0X07,0X33,0X31,0X38,0X78,0XFC,0XFE,0XF6,0XC0,0X80,
0X00,0X00,0X00,0X00,0XC0,0XC0,0X80,0X00,0XF8,0XFC,0XDE,0X0E,0X0F,0X1E,0XBE,0XFC,
0XF0,0X06,0XFF,0XFF,0XFF,0XFF,0X03,0X07,0XFF,0XFF,0XFE,0X80,0X1E,0X3F,0XFF,0XFF,
0XF9,0XF9,0XFF,0XFF,0X7F,0X1F,0X0E,0X7F,0XFF,0XFF,0XFF,0XC1,0X80,0XCE,0XCE,0XC6,
0XC0,0XFF,0XFF,0XFF,0XFF,0X3F,0X80,0XFC,0XFC,0XFD,0X7D,0X01,0XC0,0XF8,0XF8,0XF0,
0X70,0X00,0XC0,0X80,0X80,0X80,0X00,0X00,0X01,0X07,0X0E,0X3C,0XF8,0XE0,0X80,0X00,
0X00,0XFE,0XFF,0X03,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X01,0X03,0X0F,0X1F,0X7F,
0XFE,0XFC,0XF0,0XF0,0XF0,0X7F,0X7F,0X3E,0X10,0X03,0X03,0X0F,0X1F,0X1F,0X0F,0X0F,
0X03,0X04,0X07,0X07,0X07,0X07,0X06,0X00,0X07,0X07,0X07,0X0F,0X1E,0X3F,0X7F,0X7F,
0X7F,0X79,0X73,0X3F,0X3F,0X1F,0X0E,0X00,0X01,0X03,0X03,0X07,0X07,0X06,0X0E,0X0F,
0X0F,0X1F,0X3F,0X3D,0X08,0X0C,0X1F,0X1F,0X1F,0X19,0X3C,0X7F,0X7F,0X7F,0XF3,0XF0,
0XFC,0XFE,0X3F,0X1F,0X07,0X01,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XCF,0XFF,0X70,
0X00,0X03,0X1F,0X7E,0XF0,0XC0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X03,0X03,0X01,0X01,0X00,0X00,0X00,0X00,0XC0,0XC0,0X40,0X40,0X40,0X40,0XF8,0X78,
0X40,0XC0,0X40,0X58,0X58,0X40,0X40,0X40,0X00,0X00,0X00,0X00,0X00,0X80,0X80,0X80,
0X00,0X00,0X00,0X00,0X00,0X00,0X60,0XF0,0XE0,0X20,0X20,0X20,0X20,0X30,0X30,0X38,
0X18,0X10,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X01,
0X01,0X01,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X80,0XE0,0XF8,0X3F,0X0F,0X00,
0X00,0X00,0X00,0X00,0X00,0X03,0X07,0X0E,0X1C,0X38,0X30,0X60,0XE0,0XC0,0X80,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X40,0X60,0X60,0X70,0X30,0X18,0X0C,0X07,0X11,0X08,
0X3F,0X7D,0X66,0XE7,0XE3,0XC0,0XC0,0XC0,0XE0,0X60,0X00,0X00,0X00,0X03,0X03,0X03,
0X00,0X00,0X60,0X60,0X60,0X60,0X60,0X7F,0X7F,0X61,0X61,0X61,0X61,0X7F,0X7F,0X61,
0X61,0XE1,0XE0,0XE0,0XC0,0XC0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X80,0XC0,0XC0,0XE0,0X70,0X38,0X18,0X0C,0X07,0X03,0X01,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X01,0X01,0X03,
0X03,0X06,0X06,0X0C,0X0C,0X18,0X18,0X38,0X30,0X30,0X70,0X60,0X60,0XE0,0XC0,0XC0,
0XC0,0XC0,0XC0,0X80,0X80,0X80,0X81,0X80,0X80,0X80,0X80,0X80,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X80,0X80,0X80,0X80,0X80,0X80,0X80,0X80,0X80,0X80,0XC0,0XC0,
0XC0,0XC0,0XC0,0X60,0X60,0X60,0X30,0X30,0X30,0X18,0X18,0X1C,0X0C,0X0E,0X06,0X07,
0X03,0X03,0X01,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X01,0X01,0X01,0X01,0X01,0X01,0X01,0X03,0X03,0X03,0X03,0X03,0X03,0X03,
0X03,0X03,0X03,0X03,0X03,0X03,0X03,0X01,0X01,0X01,0X01,0X01,0X01,0X01,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,

};
const byte LIBLOGO60x58[480] = { 
  0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
  0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
  0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
  0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
  0X00,0X00,0XC0,0XE0,0XF8,0XFC,0XFE,0X7F,0X3F,0X0F,0X0F,0X07,0X07,0X07,0X87,0XC7,
  0XC7,0XC7,0XE7,0XEF,0XFF,0XFF,0XFF,0XEF,0XE7,0XE7,0XE7,0XE7,0XE7,0XEF,0XFF,0XFF,
  0XFF,0XEF,0XE7,0XE7,0XE7,0XE7,0XE7,0XF7,0X07,0X0F,0X3F,0X7F,0XFF,0XFE,0XFC,0XF0,
  0XC0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XD8,0XDC,0XDF,0XDF,0XDF,
  0XDF,0XD7,0X01,0X00,0X00,0X00,0X00,0X00,0X00,0X3E,0X7F,0XFF,0XFF,0XE3,0XE1,0XE1,
  0XFF,0XFF,0XFF,0XE0,0XE0,0XE0,0XE0,0XE0,0XE0,0XE0,0XFF,0XFF,0XFF,0XE0,0XE0,0XE0,
  0XE0,0XC1,0X83,0X07,0X00,0X00,0X00,0X00,0X01,0XD7,0XDF,0XDF,0XDF,0XDF,0XDC,0XD8,
  0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X01,0X03,0X0F,0X1F,0X3F,0XFF,0XFF,0XFE,0XF8,
  0XF0,0XE0,0XC0,0X80,0X00,0X00,0XF8,0XF0,0XE0,0XC0,0XC0,0XC0,0XFF,0XFF,0XFF,0XC0,
  0XC0,0XC0,0XC0,0XC0,0XC0,0XC0,0XFF,0XFF,0XFF,0XC0,0XE0,0XE1,0XF3,0XFF,0X7F,0X3F,
  0X00,0XC0,0XE0,0XF8,0XFE,0XFF,0XFF,0XFF,0X3F,0X1F,0X07,0X01,0X00,0X00,0X00,0X00,
  0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X01,0X07,0X0F,0X3F,0X7F,0XFF,
  0XFE,0XFC,0XF9,0XF3,0XC1,0X81,0X01,0X01,0XFF,0XFF,0XFF,0X01,0X01,0X01,0X01,0X01,
  0X01,0X01,0XFF,0XFF,0XFF,0X03,0X01,0X81,0XC0,0XF0,0XFC,0XFE,0XFF,0XFF,0X7F,0X3F,
  0X0F,0X07,0X03,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
  0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X03,0X07,0X0F,0X3F,
  0X7F,0XFF,0XFF,0XFF,0XFB,0XF3,0XE3,0XC3,0X03,0X03,0X03,0X03,0X03,0X83,0XC3,0XF3,
  0XF3,0XFB,0XFF,0XFF,0XFF,0X7F,0X3F,0X0F,0X03,0X01,0X00,0X00,0X00,0X00,0X00,0X00,
  0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
  0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X03,0X07,
  0X07,0X1F,0X3F,0X7F,0XFF,0XFE,0XFC,0XFC,0XFE,0XFF,0XFF,0X7F,0X3F,0X0F,0X07,0X03,
  0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
  0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
  0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
  0X00,0X03,0X07,0X0F,0X0F,0X07,0X01,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
  0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
}; 
void LCD_WrDat(byte data)
{
	byte i=8;
	//LCD_CS=0;;
	LCD_DC=1;;
  LCD_SCL=0;;
  //asm("nop");    
  while(i--)
  {
    if(data&0x80){LCD_SDA=1;}
    else{LCD_SDA=0;}
    LCD_SCL=1; 
    asm("nop");;
		//asm("nop");            
    LCD_SCL=0;;    
    data<<=1;    
  }
	//LCD_CS=1;
}
void LCD_WrCmd(byte cmd)
{
	byte i=8;
	
	//LCD_CS=0;;
	LCD_DC=0;;
  LCD_SCL=0;;
  //asm("nop");   
  while(i--)
  {
    if(cmd&0x80){LCD_SDA=1;}
    else{LCD_SDA=0;;}
    LCD_SCL=1;;
    asm("nop");;
		//asm("nop");             
    LCD_SCL=0;;    
    cmd<<=1;;   
  } 	
	//LCD_CS=1;
}
void LCD_Set_Pos(byte x, byte y)
{ 
  LCD_WrCmd(0xb0+y);
  LCD_WrCmd(((x&0xf0)>>4)|0x10);
  LCD_WrCmd((x&0x0f)|0x01); 
} 
void LCD_Fill(byte bmp_data)
{
	byte y,x;
	
	for(y=0;y<8;y++)
	{
		LCD_WrCmd(0xb0+y);
		LCD_WrCmd(0x01);
		LCD_WrCmd(0x10);
		for(x=0;x<X_WIDTH;x++)
			LCD_WrDat(bmp_data);
	}
}
void LCD_CLS(void)
{
	byte y,x;	
	for(y=0;y<8;y++)
	{
		LCD_WrCmd(0xb0+y);
		LCD_WrCmd(0x01);
		LCD_WrCmd(0x10); 
		for(x=0;x<X_WIDTH;x++)
			LCD_WrDat(0);
	}
}
void LCD_DLY_ms(word ms)
{                         
  word a;
  while(ms)
  {
    a=1335;
    while(a--);
    ms--;
  }
  return;
}
void adjust(byte a)
{
  LCD_WrCmd(a);	//指令数据0x0000~0x003f  
}
void SetStartColumn(unsigned char d)
{
	LCD_WrCmd(0x00+d%16);		// Set Lower Column Start Address for Page Addressing Mode
						//   Default => 0x00
	LCD_WrCmd(0x10+d/16);		// Set Higher Column Start Address for Page Addressing Mode
						//   Default => 0x10
}

void SetAddressingMode(unsigned char d)
{
	LCD_WrCmd(0x20);			// Set Memory Addressing Mode
	LCD_WrCmd(d);			//   Default => 0x02
						//     0x00 => Horizontal Addressing Mode
						//     0x01 => Vertical Addressing Mode
						//     0x02 => Page Addressing Mode
}

void SetColumnAddress(unsigned char a, unsigned char b)
{
	LCD_WrCmd(0x21);			// Set Column Address
	LCD_WrCmd(a);			//   Default => 0x00 (Column Start Address)
	LCD_WrCmd(b);			//   Default => 0x7F (Column End Address)
}

void SetPageAddress(unsigned char a, unsigned char b)
{
	LCD_WrCmd(0x22);			// Set Page Address
	LCD_WrCmd(a);			//   Default => 0x00 (Page Start Address)
	LCD_WrCmd(b);			//   Default => 0x07 (Page End Address)
}

void SetStartLine(unsigned char d)
{
	LCD_WrCmd(0x40|d);			// Set Display Start Line
						//   Default => 0x40 (0x00)
}

void SetContrastControl(unsigned char d)
{
	LCD_WrCmd(0x81);			// Set Contrast Control
	LCD_WrCmd(d);			//   Default => 0x7F
}

void Set_Charge_Pump(unsigned char d)
{
	LCD_WrCmd(0x8D);			// Set Charge Pump
	LCD_WrCmd(0x10|d);			//   Default => 0x10
						//     0x10 (0x00) => Disable Charge Pump
						//     0x14 (0x04) => Enable Charge Pump
}

void Set_Segment_Remap(unsigned char d)
{
	LCD_WrCmd(0xA0|d);			// Set Segment Re-Map
						//   Default => 0xA0
						//     0xA0 (0x00) => Column Address 0 Mapped to SEG0
						//     0xA1 (0x01) => Column Address 0 Mapped to SEG127
}

void Set_Entire_Display(unsigned char d)
{
	LCD_WrCmd(0xA4|d);			// Set Entire Display On / Off
						//   Default => 0xA4
						//     0xA4 (0x00) => Normal Display
						//     0xA5 (0x01) => Entire Display On
}

void Set_Inverse_Display(unsigned char d)
{
	LCD_WrCmd(0xA6|d);			// Set Inverse Display On/Off
						//   Default => 0xA6
						//     0xA6 (0x00) => Normal Display
						//     0xA7 (0x01) => Inverse Display On
}

void Set_Multiplex_Ratio(unsigned char d)
{
	LCD_WrCmd(0xA8);			// Set Multiplex Ratio
	LCD_WrCmd(d);			//   Default => 0x3F (1/64 Duty)
}

void Set_Display_On_Off(unsigned char d)
{
	LCD_WrCmd(0xAE|d);			// Set Display On/Off
						//   Default => 0xAE
						//     0xAE (0x00) => Display Off
						//     0xAF (0x01) => Display On
}

void SetStartPage(unsigned char d)
{
	LCD_WrCmd(0xB0|d);			// Set Page Start Address for Page Addressing Mode
						//   Default => 0xB0 (0x00)
}

void Set_Common_Remap(unsigned char d)
{
	LCD_WrCmd(0xC0|d);			// Set COM Output Scan Direction
						//   Default => 0xC0
						//     0xC0 (0x00) => Scan from COM0 to 63
						//     0xC8 (0x08) => Scan from COM63 to 0
}

void Set_Display_Offset(unsigned char d)
{
	LCD_WrCmd(0xD3);			// Set Display Offset
	LCD_WrCmd(d);			//   Default => 0x00
}

void Set_Display_Clock(unsigned char d)
{
	LCD_WrCmd(0xD5);			// Set Display Clock Divide Ratio / Oscillator Frequency
	LCD_WrCmd(d);			//   Default => 0x80
						//     D[3:0] => Display Clock Divider
						//     D[7:4] => Oscillator Frequency
}

void Set_Precharge_Period(unsigned char d)
{
	LCD_WrCmd(0xD9);			// Set Pre-Charge Period
	LCD_WrCmd(d);			//   Default => 0x22 (2 Display Clocks [Phase 2] / 2 Display Clocks [Phase 1])
						//     D[3:0] => Phase 1 Period in 1~15 Display Clocks
						//     D[7:4] => Phase 2 Period in 1~15 Display Clocks
}

void Set_Common_Config(unsigned char d)
{
	LCD_WrCmd(0xDA);			// Set COM Pins Hardware Configuration
	LCD_WrCmd(0x02|d);			//   Default => 0x12 (0x10)
						//     Alternative COM Pin Configuration
						//     Disable COM Left/Right Re-Map
}

void Set_VCOMH(unsigned char d)
{
	LCD_WrCmd(0xDB);			// Set VCOMH Deselect Level
	LCD_WrCmd(d);			//   Default => 0x20 (0.77*VCC)
}

void Set_NOP(void)
{
	LCD_WrCmd(0xE3);			// Command for No Operation
}

void initLCD(void)        
{
  
	LCD_SCL=1;
	//LCD_CS=1;	//预制SLK和SS为高电平   	
	
	LCD_RST=0;
	LCD_DLY_ms(50);
	LCD_RST=1;
	
	//从上电到下面开始初始化要有足够的时间，即等待RC复位完毕
  Set_Display_On_Off(0x00);		  // Display Off (0x00/0x01)
  Set_Display_Clock(0x80);		  // Set Clock as 100 Frames/Sec
  Set_Multiplex_Ratio(0x3F);		// 1/64 Duty (0x0F~0x3F)
  Set_Display_Offset(0x00);		  // Shift Mapping RAM Counter (0x00~0x3F)
  SetStartLine(0x00);			      // Set Mapping RAM Display Start Line (0x00~0x3F)
  Set_Charge_Pump(0x04);		    // Enable Embedded DC/DC Converter (0x00/0x04)
  SetAddressingMode(0x02);		  // Set Page Addressing Mode (0x00/0x01/0x02)
  Set_Segment_Remap(0x01);		  // Set SEG/Column Mapping     0x00左右反置 0x01正常
  Set_Common_Remap(0x08);			  // Set COM/Row Scan Direction 0x00上下反置 0x08正常
  Set_Common_Config(0x10);		  // Set Sequential Configuration (0x00/0x10)
  SetContrastControl(Brightness);	// Set SEG Output Current
  Set_Precharge_Period(0xF1);		// Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
  Set_VCOMH(0x40);			        // Set VCOM Deselect Level
  Set_Entire_Display(0x00);		  // Disable Entire Display On (0x00/0x01)
  Set_Inverse_Display(0x00);		// Disable Inverse Display On (0x00/0x01)  
  Set_Display_On_Off(0x01);		  // Display On (0x00/0x01)
  LCD_Fill(0x00);  //初始清屏
	LCD_Set_Pos(0,0); 
	
  /*
  LCD_WrCmd(0xae);//--turn off oled panel
  LCD_WrCmd(0x00);//---set low column address
  LCD_WrCmd(0x10);//---set high column address
  LCD_WrCmd(0x40);//--set start line address
  LCD_WrCmd(0x81);//--set contrast control register
  LCD_WrCmd(0xcf);
  LCD_WrCmd(0xa1);//--set segment re-map 95 to 0
  LCD_WrCmd(0xa6);//--set normal display
  LCD_WrCmd(0xa8);//--set multiplex ratio(1 to 64)
  LCD_WrCmd(0x3f);//--1/64 duty
  LCD_WrCmd(0xd3);//-set display offset
  LCD_WrCmd(0x00);//-not offset
  LCD_WrCmd(0xd5);//--set display clock divide ratio/oscillator frequency
  LCD_WrCmd(0x80);//--set divide ratio
  LCD_WrCmd(0xd9);//--set pre-charge period
  LCD_WrCmd(0xf1);
  LCD_WrCmd(0xda);//--set com pins hardware configuration
  LCD_WrCmd(0x12);
  LCD_WrCmd(0xdb);//--set vcomh
  LCD_WrCmd(0x40);
  LCD_WrCmd(0x8d);//--set Charge Pump enable/disable
  LCD_WrCmd(0x14);//--set(0x10) disable
  LCD_WrCmd(0xaf);//--turn on oled panel
  */
	
} 

//==============================================================
//函数名：LCD_P6x8Str(byte x,byte y,byte *p)
//功能描述：写入一组标准ASCII字符串
//参数：显示的位置（x,y），y为页范围0～7，要显示的字符串
//返回：无
//==============================================================  
void LCD_P6x8Str(byte x,byte y,char ch[])
{
  byte c=0,i=0,j=0;      
  while (ch[j]!='\0')
  {    
    c =ch[j]-32;
    if(x>126){x=0;y++;}
    LCD_Set_Pos(x,y);    
  	for(i=0;i<6;i++)     
  	  LCD_WrDat(F6x8[c][i]);  
  	x+=6;
  	j++;
  }
}


void LCD_PutROW(unsigned char x,unsigned char y,unsigned char data1,unsigned char data2)
{
	//unsigned char data1;  //data1当前点的数据 
	 
	LCD_Set_Pos(x,y); 
	//data1=data;
	LCD_WrDat(data1); 
	LCD_WrDat(data2); 
}
