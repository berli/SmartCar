#ifndef SPI_H
#define SPI_H
#include <reg52.h>

sbit CSN =	P1^0;	// 28J60-- CS(SS)
sbit SIN =	P1^1;	// MOSI
sbit SON =	P1^2;	// MISO
sbit SCKN = P1^3;	// SCK
sbit RSTN = P1^4;	// RST
sbit INTN = P3^3;	// INT 

void WriteByte(u8_t temp);
u8_t ReadByte(void);
#endif
