#ifndef _USART_INCLUDED_
#define _USART_INCLUDED_

/**********************************************************************

定义我们设备多少台  设备编号从1开始到最大数量
***********************************************************************/

#define MAXDev 0x05

#define Ticks 10            //time=Ticks*70ms  210ms更新底层设备数据，  上层设备数据更新为300ms

extern void InitGPRS();  //初始化语句



void USART3_Init(void);//串口2初始化
void USART_Init(void); //串口1初始化
void SendASC(unsigned char d);
void SendString(unsigned char *str);
extern void Timer4Init(void);		//1??@11.0592MHz

extern void Timer0Init(void);		//5925??@11.0592MHz
extern unsigned char ReFlag;
extern unsigned char *das;
extern unsigned char Maxdata[MAXDev][89];//数组二维保存数据


extern unsigned char SendOrNot_Flag[5];//标志位我们的选择是否发送


//extern unsigned char data da[49];
//extern unsigned char data dare[49];
//extern unsigned char data dack[8];
#endif