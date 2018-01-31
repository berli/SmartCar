#include	"STC15Fxxxx.H"
#include  "RFIDSERIAL.h"  //串口协议
#include  "PUBLIC.h"

//1
#define S1_S0 0x40              //P_SW1.6
#define S1_S1 0x80              //P_SW1.7

//2
#define S2RI  0x01              //S2CON.0
#define S2TI  0x02              //S2CON.1
#define S2RB8 0x04              //S2CON.2
#define S2TB8 0x08              //S2CON.3
#define S2_S0 0x01              //P_SW2.0

//3


//4
#define S4RI  0x01              //S4CON.0
#define S4TI  0x02              //S4CON.1
#define S4RB8 0x04              //S4CON.2
#define S4TB8 0x08              //S4CON.3
#define S4_S0 0x04              //P_SW2.2



/*************	本地常量声明	**************/
#define MAIN_Fosc		11059200L	//定义主时钟
#define	RX1_Lenth		32			//串口接收缓冲长度
#define	BaudRate1		115200UL	//选择波特率

#define	Timer1_Reload	(65536UL -(MAIN_Fosc / 4 / BaudRate1))		//Timer 1 重装值， 对应300KHZ
#define	Timer2_Reload	(65536UL -(MAIN_Fosc / 4 / BaudRate1))		//Timer 2 重装值， 对应300KHZ


uchar all=0;           
uchar go=0;            //10位数据
uchar Checked=0;       //用于标志是否接受完数据

uchar DATA_SEND[10]={ 0x7E, 0x00,     0,  0,      0,0,0,     0x08,      0,       0x7E};
uchar DATA_GET[10]= { 0x7E, 0x00,     0,  0,      0,0,0,     0x08,      0,       0x7E};

/*********************************************************************
                *函数的声明以便自己调用
**********************************************************************/

void UartInit1(void);
void UartInit2(void);
void UartInit3(void);
void UartInit4(void);

void Send_Data();
void Get_Data_ONE();
void Get_Data_TWO();  //第二串口 接收到 的一楼数据
void Get_Data_THREE();
void Get_Data_FOUR();  

//void Must_Modify();
//void CHECK_SUM();
void Receive_Data();
void CHECK_FUN();


/*******************************************************************************************
*     串口发送前 从新赋值给打包数组 及更新数据
****************************************************************************************/
/*
void Must_Modify()
{
	DATA_SEND[0]=DATA_PUBLIC[Head];     //数据头
	DATA_SEND[1]=DATA_PUBLIC[RoomID];  //放假号
	DATA_SEND[2]=DATA_PUBLIC[DEviceID];//设备号
	DATA_SEND[3]=DATA_PUBLIC[Operate]; //操作
	DATA_SEND[4]=DATA_PUBLIC[Dateh];   //整数高两位
	DATA_SEND[5]=DATA_PUBLIC[Datel];  //整数地两位
	DATA_SEND[6]=DATA_PUBLIC[Dated];   //小数两位
	DATA_SEND[7]=DATA_PUBLIC[Save];    //保留
	DATA_SEND[9]=DATA_PUBLIC[Teal]; //数据尾
}

*/
/*******************************************************************************
          中断发送函数
********************************************************************************/

void Send_Data()
{
	 uint i=0;
	 if(FLAG_Recd==2)
	 {
		  for(i=0;i<10;i++)          //发送10位数据位  发送至PC端
			 {
					SBUF=DATA_Serial2[i];
					while(!TI);
					TI=0;
			 }
   }
	 else if(FLAG_Recd==3)
	 {
		  for(i=0;i<10;i++)          //发送10位数据位  发送至PC端
			 {
					SBUF=DATA_Serial3[i];
					while(!TI);
					TI=0;
			 }
   }
	 else if(FLAG_Recd==4)
	 {
		  for(i=0;i<10;i++)          //发送10位数据位  发送至PC端
			 {
					SBUF=DATA_Serial4[i];
					while(!TI);
					TI=0;
			 }
   }
	 
	 FLAG_Recd=0;
}


/*******************************************************************************
          中断接收数据函数
********************************************************************************/

void Get_Data_ONE()//接收到的数据  现阶段只有头尾 和操作数  其他的没有校验 
{
	 Receive_Data();
	 CHECK_FUN();
}


//第二串口  接收到的数据

void Get_Data_TWO()  
{
	if((all==0)&&(S2BUF==0x7E))// 判断第一个是不是0x7e  不确定是不是尾部7e
	   {go=1;}
	if((go==1)&&(all==1)&&(S2BUF==0X7E))//第二个7e
	   {all=0;}                        //只把all清零，用来保存第二个数字
	if(go==1)  //10个字符可以运行了
	{
		DATA_Serial2[all]=S2BUF;
	 	all++;
		if(all==10)
		{
			go=0;         //可以接受第一个字符
			all=0;
			FLAG_Recd=2;//标志为  串口2
			TI=1;
			//Checked=1;  //我们需要的 一个数据包接收完毕
	  }
  }
}


void Get_Data_THREE()  
{
	if((all==0)&&(S3BUF==0x7E))// 判断第一个是不是0x7e  不确定是不是尾部7e
	   {go=1;}
	if((go==1)&&(all==1)&&(S3BUF==0X7E))//第二个7e
	   {all=0;}                        //只把all清零，用来保存第二个数字
	if(go==1)  //10个字符可以运行了
	{
		DATA_Serial3[all]=S3BUF;
	 	all++;
		if(all==10)
		{
			go=0;         //可以接受第一个字符
			all=0;
			FLAG_Recd=3;//标志为  串口2
			TI=1;
			//Checked=1;  //我们需要的 一个数据包接收完毕
	  }
  }
}


void Get_Data_FOUR()  
{
	if((all==0)&&(S4BUF==0x7E))// 判断第一个是不是0x7e  不确定是不是尾部7e
	   {go=1;}
	if((go==1)&&(all==1)&&(S4BUF==0X7E))//第二个7e
	   {all=0;}                        //只把all清零，用来保存第二个数字
	if(go==1)  //10个字符可以运行了
	{
		DATA_Serial4[all]=S4BUF;
	 	all++;
		if(all==10)
		{
			go=0;         //可以接受第一个字符
			all=0;
			FLAG_Recd=4;//标志为  串口2
			TI=1;
			//Checked=1;  //我们需要的 一个数据包接收完毕
	  }
  }
}
/***************************************************************
             用于一位一位的接收数据
*******************************************************************/

void Receive_Data()//接收到的数据  现阶段只有头尾 和操作数  其他的没有校验 
{
	if((all==0)&&(SBUF==0x7E))// 判断第一个是不是0x7e  不确定是不是尾部7e
	   {go=1;}
	if((go==1)&&(all==1)&&(SBUF==0X7E))//第二个7e
	   {all=0;}                        //只把all清零，用来保存第二个数字
	if(go==1)  //10个字符可以运行了
	{
		DATA_GET[all]=SBUF;
	 	all++;
		if(all==10)
		{
			go=0;         //可以接受第一个字符
			all=0;
			Checked=1;  //我们需要的 一个数据包接收完毕
	  }
  }
}


/***************************************************************
              用于校验 查看命令
*******************************************************************/

void CHECK_FUN()
{
	if(Checked==1)//数据接收完毕
	{
		Checked=0;
		if((DATA_GET[0]==0X7E)&&(DATA_GET[9]==0X7E))//是否是对的  &&(DATA_GET[1]==0x10)
		{
			switch(DATA_GET[1])   //才看指令
			{
				case 0X10:   S2CON |= S2TI;  break; //发送给一楼 0x10
				case 0X20:   S3CON |= 2;     break; //发送给一2楼 0x20
				case 0X30:   S4CON |= S4TI;  break; //发送给3一楼 0x30
				default:      break;
			}
		}
		else
		{
			char k=0;
			for(k=0;k<10;k++)
			   { DATA_GET[k]=0;}//指令错误 清空数据
    }
	}
}


/***************************************************************
              求校验和   用于发送    发上去的数据全部校验
*******************************************************************/
/*
void CHECK_SUM()
{
	uchar check_sum=0x00;
	uint j=1;
	for(j=1;j<8;j++)
	{
			if(DATA_SEND[j]>=0x30)
			{
				check_sum+=DATA_SEND[j];           //数字大于等于0x30的减去0x30 再相加
				check_sum-=0x30;
			}
			else
				check_sum+=DATA_SEND[j];	
  }
	check_sum=~check_sum+1;           //校验和取反加1
	DATA_SEND[8]=check_sum;  //校验位
}

*/

/*************************************************************  
	   1   *初始化串口中断
**********************************************************/

void UartInit1(void)             
{
    ACC = P_SW1;
    ACC &= ~(1 | S1_S1);    //S1_S0=0 S1_S1=0
    P_SW1 = ACC;                //(P3.0/RxD, P3.1/TxD) 切换为(P3.6/RxD_2, P3.7/TxD_2)
    SCON = 0x50;         
    T2L = (65536 - (MAIN_Fosc/4/BaudRate1));   //设置波特率重装值
    T2H = (65536 - (MAIN_Fosc/4/BaudRate1))>>8;
    AUXR = 0x14;                //T2为1T模式, 并启动定时器2
    AUXR |= 0x01;               //选择定时器2为串口1的波特率发生器
    ES = 1;                     //使能串口1中断
    EA = 1;
}

void UartInit2(void)             
{
	  P_SW2 &= ~S2_S0;            //S2_S0=0 (P1.0/RxD2, P1.1/TxD2)
    S2CON = 0x50;               //8位可变波特率
    T2L = (65536 - (MAIN_Fosc/4/BaudRate1));   //设置波特率重装值
    T2H = (65536 - (MAIN_Fosc/4/BaudRate1))>>8;
    AUXR = 0x14;                //T2为1T模式, 并启动定时器2
    IE2 |= 0x01;                 //使能串口2中断
    EA = 1;
}
//						          7      6      5      4      3      2     1     0        Reset Value
//sfr S2CON = 0x9A;		S2SM0    -    S2SM2  S2REN  S2TB8  S2RB8  S2TI  S2RI      00000000B		 //S2 Control

void UartInit3(void)             
{
	S3CON &= ~(1<<7);	//8位数据
	S3CON &= 0xBF;		//选择定时器2
	S3_USE_P00P01();		//UART3 使用P0.0 P0.1口	默认
	AUXR = 0x14;                //T2为1T模式, 并启动定时器2
  TH2 = (u8)(Timer2_Reload >> 8);
	TL2 = (u8)Timer2_Reload;
	IE2  |= (1<<3);		//允许中短es3
	S3CON |=  (1<<4);	//允许接收
	AUXR |=  (1<<4);	//Timer run enable
	EA = 1;		//允许全局中断
}

void UartInit4(void)             
{
		P_SW2 &= ~S4_S0;            //S4_S0=0 (P0.2/RxD4, P0.3/TxD4)
    S4CON = 0x10;               //8位可变波特率
    T2L = (65536 - (MAIN_Fosc/4/BaudRate1));   //设置波特率重装值
    T2H = (65536 - (MAIN_Fosc/4/BaudRate1))>>8;
    AUXR = 0x14;                //T2为1T模式, 并启动定时器2
    IE2 |= 0x10;                 //使能串口4中断
    EA = 1;
}

/*******************************************************************************************
*   1   串口中断函数 发送和接收数据包
****************************************************************************************/

void Package_Send_AND_Get() interrupt UART1_VECTOR
{
//	FLAG_Send  FLAG_Recd   BUSY
    if (RI)
    {
        while(!RI);
				RI=0;
	    	Get_Data_ONE();
		   
    }
    if (TI)
    {
			  while(!TI);
        TI = 0;                 //清除TI位
				Send_Data();
    }
}


/*******************************************************************************************
*   2 串口中断函数 发送和接收数据包
****************************************************************************************/

void Package_Send_AND_Get2 (void) interrupt UART2_VECTOR
{
	  char k=0;
    if (S2CON & S2RI)
    {
        S2CON &= ~S2RI;         //清除S2RI位
				Get_Data_TWO();
    }
    if (S2CON & S2TI)
    {
        S2CON &= ~S2TI;         //清除S2TI位
				for(k=0;k<10;k++)
				{
						S2BUF = DATA_GET[k];               
						while(!(S2CON & S2TI));
						S2CON &= ~S2TI;         //清除S2TI位
        }
   }
}


/*******************************************************************************************
*      3 串口中断函数 发送和接收数据包
****************************************************************************************/

void Package_Send_AND_Get3 (void) interrupt UART3_VECTOR
{
	char k=0;
  if((S3CON & 1) != 0)
	{
		S3CON &= ~1;                       //清除标志位
    Get_Data_THREE();
  	}
  if((S3CON & 2) != 0)
	{
			S3CON &= ~2;                        //清除标志位
			for(k=0;k<10;k++)
			{
					S3BUF=DATA_GET[k];      //dec_to_hex(RX3_Buffer);		//发送  二进制转换为十进制
					while(!((S3CON & 2) != 0));
					S3CON &= ~2;                     //清除标志位
  	  }
   }
	
}


void Package_Send_AND_Get4 (void) interrupt UART4_VECTOR
{
  	char k=0;
	 if (S4CON & S4RI)
    {
        S4CON &= ~S4RI;         //清除S4RI位
        Get_Data_FOUR(); 
    }
    if (S4CON & S4TI)
    {
        S4CON &= ~S4TI;         //清除S4TI位
				for(k=0;k<10;k++)
				{
						S4BUF=DATA_GET[k];            
						while(!(S4CON & S4TI));
						S4CON &= ~S4TI;         //清除S4TI位
				}
    }
}

