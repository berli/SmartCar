#ifndef __PUBLIC_H_
#define __PUBLIC_H_

extern unsigned char DATA_PUBLIC[10];
extern unsigned char DATA_Serial1[10];
extern unsigned char DATA_Serial2[10];
extern unsigned char DATA_Serial3[10];
extern unsigned char DATA_Serial4[10];

extern unsigned char FLAG_Send;  //用于查看是哪一个促发了中断
extern unsigned char FLAG_Recd;  //用于查看是哪一个促发了中断
extern unsigned char BUSY;  //查看当前的哪一个促发啦

 
extern unsigned char Head;
extern unsigned char RoomID;
extern unsigned char DEviceID;
extern unsigned char Operate;
extern unsigned char Dateh;
extern unsigned char Datel;
extern unsigned char Dated;
extern unsigned char Save;
extern unsigned char Check;
extern unsigned char Teal;

//extern unsigned char hex_to_dec(unsigned char);
//extern unsigned char dec_to_hex(unsigned char);

#endif