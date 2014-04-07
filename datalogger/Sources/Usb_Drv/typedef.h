/*************************************************************************
 * DISCLAIMER *
 * Services performed by FREESCALE in this matter are performed          *
 * AS IS and without any warranty. CUSTOMER retains the final decision   *
 * relative to the total design and functionality of the end product.    *
 * FREESCALE neither guarantees nor will be held liable by CUSTOMER      *
 * for the success of this project. FREESCALE disclaims all warranties,  *
 * express, implied or statutory including, but not limited to,          *
 * implied warranty of merchantability or fitness for a particular       *
 * purpose on any hardware, software ore advise supplied to the project  *
 * by FREESCALE, and or any product resulting from FREESCALE services.   *
 * In no event shall FREESCALE be liable for incidental or consequential *
 * damages arising out of this agreement. CUSTOMER agrees to hold        *
 * FREESCALE harmless against any and all claims demands or actions      *
 * by anyone on account of any damage, or injury, whether commercial,    *
 * contractual, or tortuous, rising directly or indirectly as a result   *
 * of the advise or assistance supplied CUSTOMER in connection with      *
 * product, services or goods supplied under this Agreement.             *
 *************************************************************************/
/*************************************************************************************************
 * File name   : typedef.h
 *
 * Description : This file delcares some different data type 
 *               
 *
 * History     :
 * 04/01/2007  : Initial Development
 * 
 *************************************************************************************************/


#ifndef _TYPEDEF_H
#define _TYPEDEF_H

typedef unsigned char   byte;           /* 8-bit*/
typedef unsigned int    word;           /* 16-bit*/
typedef unsigned long   dword;          /* 32-bit*/

typedef union _BYTE
{
    byte _byte;
    struct
    {
        unsigned b0:1;
        unsigned b1:1;
        unsigned b2:1;
        unsigned b3:1;
        unsigned b4:1;
        unsigned b5:1;
        unsigned b6:1;
        unsigned b7:1;
    }Bit;
} BYTE;

typedef union _WORD
{
    word _word;
    struct
    {
        byte byte1;
        byte byte0;
    }_byte;
    struct
    {
        BYTE HighB;
        BYTE LowB;
    }_Byte;
} WORD;

#define LSB(a)      ((a)._byte.byte0)
#define MSB(a)      ((a)._byte.byte1)

typedef union _DWORD
{
    dword _dword;
    struct
    {
        byte byte3;
        byte byte2;
        byte byte1;
        byte byte0;
    }_byte;
    struct
    {
        word word1;
        word word0;
    }_word;
    struct
    {
        BYTE Byte3;
        BYTE Byte2;
        BYTE Byte1;
        BYTE Byte0;
    }_Byte;
    struct
    {
        WORD Word1;
        WORD Word0;
    }_Word;
} DWORD;

#define LOWER_LSB(a)    ((a)._byte.byte0)
#define LOWER_MSB(a)    ((a)._byte.byte1)
#define UPPER_LSB(a)    ((a)._byte.byte2)
#define UPPER_MSB(a)    ((a)._byte.byte3)

typedef void(* pFunc)(void);


#endif            
