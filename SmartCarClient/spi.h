#ifndef SPI_H
#define SPI_H
//#include <reg52.h>
#include "STC15Fxxxx.h"

#include "spi.h"
//#include "struct.h"

//sfr P4 = 0xe8;

//sbit VCC1 = P2^0;// VCC1	NO USE

sbit CSN = P1^0	;// 28J60-- CS
sbit RSTN = P1^1 ; //RST
sbit SIN = P1^3 ;// MOSI
sbit SON = P1^4	;// MISO
sbit SCKN = P1^5 ; // SCK
sbit INTN = P3^3 ; // INT 

void WriteByte(u8_t temp);
u8_t ReadByte(void);

#endif
