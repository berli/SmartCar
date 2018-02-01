/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC15F4K60S4 系列 定时器1用作串口1的波特率发生器举例------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966-------------------------*/
/* --- Web: www.STCMCU.com --------------------------------------------*/
/* --- Web: www.GXWMCU.com --------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序        */
/* 如果要在文章中应用此代码,请在文章中注明使用了STC的资料及程序        */
/*---------------------------------------------------------------------*/

//本示例在Keil开发环境下请选择Intel的8058芯片型号进行编译
//若无特别说明,工作频率一般为11.0592MHz


#include "STC15W4K58S4.h"

#include "intrins.h"
#include <string.h>  // 字符串处理头文件

sbit LED = P3 ^ 2;  // 对应硬件连接
sbit LOUND = P5 ^ 4;  // 对应硬件连接

typedef unsigned char BYTE;
typedef unsigned int WORD;

BYTE DATA_LENGTH =7;

BYTE DATA_SEND[]={ 0x7E, 0x00,     0,  0,      0,      0,       0x7E};
BYTE DATA_GET[]= { 0x7E, 0x00,     0,  0,      0,      0,       0x7E};

BYTE all=0;           



#define FOSC 11059200L          //系统频率
#define BAUD 115200             //串口波特率

#define NONE_PARITY     0       //无校验
#define ODD_PARITY      1       //奇校验
#define EVEN_PARITY     2       //偶校验
#define MARK_PARITY     3       //标记校验
#define SPACE_PARITY    4       //空白校验

#define PARITYBIT NONE_PARITY   //定义校验位


#define S1_S0 0x40              //P_SW1.6
#define S1_S1 0x80              //P_SW1.7


bit busy;

void SendData(BYTE dat);
void  SendString(char *s);

void UART_TC (unsigned char *str);
void UART_T (unsigned char UART_data); //定义串口发送数据变量
void Get_Data_ONE();
void DELAY_MS(unsigned int timeout);		//@11.0592MHz
void server();


void main()
{
    P0M0 = 0x00;
    P0M1 = 0x00;
    P1M0 = 0x00;
    P1M1 = 0x00;
    P2M0 = 0x00;
    P2M1 = 0x00;
    P3M0 = 0x00;
    P3M1 = 0x00;
    P4M0 = 0x00;
    P4M1 = 0x00;
    P5M0 = 0x00;
    P5M1 = 0x00;
    P6M0 = 0x00;
    P6M1 = 0x00;
    P7M0 = 0x00;
    P7M1 = 0x00;

 //   ACC = P_SW1;
//    ACC &= ~(S1_S0 | S1_S1);    //S1_S0=0 S1_S1=0
//    P_SW1 = ACC;                //(P3.0/RxD, P3.1/TxD)
    
  ACC = P_SW1;  ACC &= ~(S1_S0 | S1_S1);    //S1_S0=1 S1_S1=0
  ACC |= S1_S0;               //(P3.6/RxD_2, P3.7/TxD_2)
  P_SW1 = ACC;  
  
//  ACC = P_SW1;
//  ACC &= ~(S1_S0 | S1_S1);    //S1_S0=0 S1_S1=1
//  ACC |= S1_S1;               //(P1.6/RxD_3, P1.7/TxD_3)
//  P_SW1 = ACC;  

#if (PARITYBIT == NONE_PARITY)
    SCON = 0x50;                //8位可变波特率
#elif (PARITYBIT == ODD_PARITY) || (PARITYBIT == EVEN_PARITY) || (PARITYBIT == MARK_PARITY)
    SCON = 0xda;                //9位可变波特率,校验位初始为1
#elif (PARITYBIT == SPACE_PARITY)
    SCON = 0xd2;                //9位可变波特率,校验位初始为0
#endif

    AUXR = 0x40;                //定时器1为1T模式
    TMOD = 0x00;                //定时器1为模式0(16位自动重载)
    TL1 = (65536 - (FOSC/4/BAUD));   //设置波特率重装值
    TH1 = (65536 - (FOSC/4/BAUD))>>8;
    TR1 = 1;                    //定时器1开始启动
    ES = 1;                     //使能串口中断
    EA = 1;
			

  //  UART_TC("STC15F2K60S2\r\nUart Test !\r\n\0");
	
	LED = 0;
	LOUND = 0;
		
		DELAY_MS(2000);
		 server();
all=0;
DELAY_MS(2000);
			SendString(DATA_SEND);

    while(1){



DELAY_MS(2000);
			SendString(DATA_GET);
};
}

/*----------------------------
UART 中断服务程序
-----------------------------*/
void Uart() interrupt 4 using 1
{
    if (RI)
    {
        while(!RI);
				RI=0;
	    	Get_Data_ONE();
					busy = 0; 
		   
    }
		   if (TI)
    {
			  while(!TI);
        TI = 0;                 //清除TI位
				busy = 0;               //清忙标志
    }
}

/*----------------------------
发送串口数据
----------------------------*/
void SendData(BYTE dat)
{
    while (busy);               //等待前面的数据发送完成
    ACC = dat;                  //获取校验位P (PSW.0)
    if (P)                      //根据P来设置校验位
    {
#if (PARITYBIT == ODD_PARITY)
        TB8 = 0;                //设置校验位为0
#elif (PARITYBIT == EVEN_PARITY)
        TB8 = 1;                //设置校验位为1
#endif
    }
    else
    {
#if (PARITYBIT == ODD_PARITY)
        TB8 = 1;                //设置校验位为1
#elif (PARITYBIT == EVEN_PARITY)
        TB8 = 0;                //设置校验位为0
#endif
    }
    busy = 1;
    SBUF = ACC;                 //写数据到UART数据寄存器
}


/*----------------------------
发送字符串
----------------------------*/

void  SendString(char *s)
{
	 unsigned int i=0;
		 
		   for(i=0;i<DATA_LENGTH;i++)          //发test数据 发送至PC端
			 {
					SBUF=s[i];
					while(!TI);
					TI=0;
			 }
}

void UART_T (unsigned char UART_data){ //定义串口发送数据变量
	SBUF = UART_data;	//将接收的数据发送回去
	while(TI == 0);		//检查发送中断标志位
	TI = 0;			//令发送中断标志位为0（软件清零）
}

void UART_TC (unsigned char *str){
	while(*str != '\0'){
		UART_T(*str);
		*str++;
	}
	*str = 0;
}


//串口  接收到的数据

void Get_Data_ONE()  
{

		DATA_GET[all]=SBUF;
	 	all++;
		if(all==DATA_LENGTH)
		{
			all=0;
			if(DATA_GET[1]==0x03 && DATA_GET[4]==0x02){
				LED = 1;
			}else	if(DATA_GET[1]==0x03 && DATA_GET[4]==0x01){
				LED = 0;
			}else if(DATA_GET[1]==0x02 && DATA_GET[4]==0x02){
				LOUND = 1;
			}else	if(DATA_GET[1]==0x02 && DATA_GET[4]==0x01){
				LOUND = 0;
			}
				
			SendString(DATA_GET);

	  
			}
}

void DELAY_1MS(){
unsigned char i, j;

	_nop_();
	_nop_();
	_nop_();
	i = 11;
	j = 190;
	do
	{
		while (--j);
	} while (--i);
	

}

void DELAY_MS(unsigned int timeout)		//@11.0592MHz
{
		int t = 0;
	while (t < timeout)
	{
		t++;
	
	DELAY_1MS();
	
}
}

void server(){


	UART_TC("+++\0"); // 退出透传模式
	DELAY_MS( 1500);

	UART_TC("AT\r\n\0");	// AT指令测试
	DELAY_MS(1500);

	//UART_TC("AT+CWSTARTSMART\r\n\0"); // 开始智能配网模式
	//DELAY_MS(1000);
//	LED = 0; // 配网指示灯亮起
//	DELAY_MS( 30000); // 链接成功

	//UART_TC("AT+CWSTOPSMART\r\n\0"); // 结束智能配网模式，释放模块资源(必须)
 //	DELAY_MS( 1000);
	//LED = 1; // 配网指示灯熄灭

	UART_TC("AT+CIPMUX=0\r\n\0");  // 设置单连接模式
	DELAY_MS(1500);

	UART_TC("AT+CIPSTART=\"TCP\",\"192.168.0.104\",4001\r\n\0");	// 连接到指定TCP服务器
	DELAY_MS( 3500);

	UART_TC("AT+CIPMODE=1\r\n\0"); // 设置透传模式
	DELAY_MS( 1500);

	UART_TC("AT+SAVETRANSLINK=1,\"192.168.0.104\",4001,\"TCP\"\r\n\0"); // 保存TCP连接到flash，实现上电透传
	DELAY_MS(1500);

	UART_TC("AT+CIPSEND\r\n\0");	 // 进入透传模式
	DELAY_MS( 1500);


}
