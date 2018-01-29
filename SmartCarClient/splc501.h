#ifndef _SPLC501_H
#define _SPLC501_H
#include <reg52.H>
#include <intrins.h>

#define  LCD_IO_DATA     P0
#define  LCD_IO_CONTROL  P2
#define	 C_RST_Pin	   1<<2//0x04 P2^2
#define  C_AOP_Pin     1<<5//0x20 P2^5
#define  C_EP_Pin      1<<7//0x80 P2^7
#define  C_RWP_Pin     1<<6//0x40 P2^6
#define  Busy		   0x80	

/*
数据线接 P0
RESET P1_0
A0 P1_4
RW P1_2
EP P1_3
*/
	
//========================================================

//函数声明
//========================================================
 void InitGraphic(void);
 void sendorder(void);
 void senddata(void);
 void setpage( int a);
 void setaddr( int addr);
 void ClearScreen( int a);
 void putstring(char a,char b,char *n);
 void x(char n);

#endif
