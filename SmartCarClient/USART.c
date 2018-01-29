#include "USART.h"
#include "STC15Fxxxx.h"
#include "example0.h"

//1
#define S1_S0 0x40              //P_SW1.6
#define S1_S1 0x80              //P_SW1.7

//2
#define S2RI  0x01              //S2CON.0
#define S2TI  0x02              //S2CON.1
#define S2RB8 0x04              //S2CON.2
#define S2TB8 0x08              //S2CON.3
#define S2_S0 0x01              //P_SW2.0

#define FOSC 11059200L          //系统频率
#define BAUD 115200             //串口波特率

#define NONE_PARITY     0       //无校验

#define PARITYBIT EVEN_PARITY   //定义校验位

#define S3RI  0x01              //S3CON.0
#define S3TI  0x02              //S3CON.1
#define S3RB8 0x04              //S3CON.2
#define S3TB8 0x08              //S3CON.3

#define S3_S0 0x02              //P_SW2.1


static unsigned int ccd=0;        //用于计数，接收电表上传的数据多少个 默认49
static unsigned char ckcount=0; //用于计数28次就2s;
static unsigned char cuun=0;
static unsigned char onescheck=1;
static unsigned char sbnumb=0;

static unsigned int RestartFlag=0;

unsigned char *das;
unsigned char ReFlag=0;
unsigned char T0Flag=0;

////da用于发送出去的函数 发送数据
//unsigned char data da[49]={0x03, 0x03 , 0x2C , 0x00 , 0x00 , 0x00 , 0x00 , 0x08 , 0xFE , 0x00 , 0x00 ,
//                           0x08 , 0xFE, 0x08 , 0xFE , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 ,
//                           0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 ,
//                           0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00,
//                           0x00 , 0x00 , 0x00 , 0xC3 , 0x02};
//二维数组保存传输数据  60台设备 最大值
//extern unsigned char Maxdata[MAXDev][49]={0x00};


//获取数据指令   自定义循环发送
unsigned char data dack[8]={0x00,0x03,0x00,0x17,0X00,0x2A,0x00,0x00};


unsigned char data dare[89]={0x00};//接收我们485传来的数据
unsigned char data CRC[2]={0x00};//求出来的校验和
unsigned int Program[MAXDev][0x09]={0};//我们的二维数组保存我们所有数据，解析后的
unsigned char Maxdata[MAXDev][89]={0x00};//数组二维保存数据

unsigned char GPRS_Flag[5][2]={0x00};//二维数组保存GPRS的标志位
unsigned char SendOrNot_Flag[5]={0x01,0x01,0x01,0x01,0x01};//标志位我们的选择是否发送




/*****************************************************************
    函数声明
****************************************************************/
void CheckCRC(unsigned char *message,unsigned char *CRC);
void GETDATA(unsigned char *values);
void GetCRC(unsigned char message[],unsigned char *CRC);
void Delay50us();		//@11.0592MHz
void Check_Mas(unsigned char sbid);

void InitGPRS();  //初始化语句

void Delay1000ms();  //@11.0592MHz

void send_GPRS();//向GPRS发送数据
void write_GPRS(unsigned char a);   //向GPRS发短信

void CheckCRC(unsigned char *message,unsigned char *CRC)
{
    unsigned int CRCFull = 0xFFFF;
    unsigned char CRCHigh = 0xFF, CRCLow = 0xFF;
    unsigned int CRCLSB;
    char i,j;
    for ( i = 0; i < 47; i++)
    {
         CRCFull = (unsigned int)(CRCFull ^ *(message+i));
         for ( j = 0; j < 8; j++)
          {
                CRCLSB = (unsigned int)(CRCFull & 0x0001);
                CRCFull = (unsigned int)((CRCFull >> 1) & 0x7FFF);
                if (CRCLSB == 1)
                    CRCFull =(CRCFull ^ 0xA001);
          }
     }
     *CRC= CRCHigh = (unsigned int)((CRCFull >> 8) & 0xFF);
	   ++CRC;
		*CRC= CRCLow = (unsigned int)(CRCFull & 0xFF);
}


void GETDATA(unsigned char *values)
{
	if(*(values+0) <= 0x05)
	{
		if((*(values+87)==CRC[1])&&(*(values+88)==CRC[0]))
		{
			 if (*(values+1) == 3)
			 {
							Program[*(values+0)-1][0] = *(values+15) * 256 + *(values+16);
							Program[*(values+0)-1][1] = *(values+17) * 256 + *(values+18);
							Program[*(values+0)-1][2] = *(values+19) * 256 + *(values+20);
							Program[*(values+0)-1][3] = *(values+3) * 256 + *(values+4);
							Program[*(values+0)-1][4] = *(values+5) * 256 + *(values+6);
							Program[*(values+0)-1][5] = *(values+7) * 256 + *(values+8);
							Program[*(values+0)-1][6] = *(values+27) * 256 + *(values+28);
							Program[*(values+0)-1][7] = *(values+35) * 256 + *(values+36);
							Program[*(values+0)-1][8] = *(values+45) * 256 + *(values+46);
			 }
		}
	}
}


//发送的时候crc校验取值，一维数组

void GetCRC(unsigned char message[],unsigned char *CRC)
{
    unsigned int CRCFull = 0xFFFF;
    unsigned char CRCHigh = 0xFF, CRCLow = 0xFF;
    unsigned int CRCLSB;
    char i,j;
    for ( i = 0; i < 6; i++)
    {
        CRCFull = (unsigned int)(CRCFull ^ message[i]);
        for ( j = 0; j < 8; j++)
        {
             CRCLSB = (unsigned int)(CRCFull & 0x0001);
             CRCFull = (unsigned int)((CRCFull >> 1) & 0x7FFF);
             if (CRCLSB == 1)
                CRCFull =(CRCFull ^ 0xA001);
        }
     }
     *CRC= CRCHigh = (unsigned int)((CRCFull >> 8) & 0xFF);
	   ++CRC;
		*CRC= CRCLow = (unsigned int)(CRCFull & 0xFF);
}


void Delay50us()		//@11.0592MHz
{
		unsigned char i, j;

		_nop_();
		i = 1;
		j = 134;
		do
		{
			while (--j);
		} while (--i);
}


//串口初始化1
void USART_Init()
{
		  P0_2=0;

	SCON = 0x50;		//8???,?????
	AUXR |= 0x40;		//???1???Fosc,?1T
	AUXR &= 0xFE;		//??1?????1???????
	TMOD &= 0x0F;		//?????1?16???????
	TL1 = 0xE8;		//??????
	TH1 = 0xFF;		//??????
	ET1 = 0;		//?????1??
	TR1 = 1;		//?????1
	REN = 1;	//允许接收
	PS=1;         //高级中断优先
  ES = 1;				//开串行中断
	EA = 1;				//开总中断
}

void USART3_Init(void)		//9600bps@11.0592MHz  串口2与485通信。modbus通信协议
{
	S3CON &= ~(1<<7);	//8位数据
	S3CON &= 0xBF;		//选择定时器2
	S3_USE_P00P01();		//UART3 使用P0.0 P0.1口	默认
	AUXR |= 0x14;                //T2为1T模式, 并启动定时器2
  TH2 = (u8)((65536UL -(11059200UL / 4 / 9600L)) >> 8);
	TL2 = (u8)(65536UL -(11059200UL / 4 / 9600L));
	IE2  |= (1<<3);		//允许中短es3
	S3CON |=  (1<<4);	//允许接收
	AUXR |=  (1<<4);	//Timer run enable
	EA = 1;		//允许全局中断
}

void Timer4Init(void)		//70ms @11.0592MHz
{
	T4T3M &= 0xDF;		//?????12T??
	T4L = 0x00;		//??????
	T4H = 0x04;		//??????
	T4T3M |= 0x80;		//???4????
	IE2 |= 0x40;                    //????4??
}

void Timer0Init(void)		//70ms@11.0592MHz
{
  AUXR &= 0x7F;		//?????12T??
	TMOD &= 0xF0;		//???????
	TL0 = 0x00;		//??????
	TH0 = 0x28;		//??????
	TF0 = 0;		//??TF0??
	TR0 = 1;		//???0????
	PT0=0;
	ET0 = 0;//不允许中断
  EA=1;	
}


//发送一个字符,ASCII码
void SendASC(unsigned char d)
{
	SBUF=d;
	while(!TI);	//等待发送完成,TI置位
	TI=0;
}

//发送字符串
void SendString(unsigned char *str)	
{
	while(*str)
	{
		SendASC(*str) ;
		str++;
	}
}


//串口中断服务程序
void USART_Interrupt(void)  interrupt UART1_VECTOR 	  //放在这里 和放在main（）里面是一样的
{
//	unsigned char da,db;
	//EA=0; 
  if(RI) 			//必须判断RI(接收中断标志)是否为1
  {
    RI=0;
		//可以用于发送GPRS电话和短信
//		
//  	da=(unsigned char)((Program[2][5] >> 8) & 0xFF);
//    db=(unsigned char)(Program[2][5] & 0xFF);

//	  SendASC(da);
//		SendASC(db);
		
  }
	if(TI)
	{
		TI=0;
  }
//	EA=1;
}


/*----------------------------
UART3 中断服务程序
-----------------------------*/

void Package_Send_AND_Get3 (void) interrupt UART3_VECTOR
{
  if((S3CON & 1) != 0)
	{
		S3CON &= ~1;                       //清除标志位
		dare[ccd]=S3BUF;                   //接收到的数据放入dare，用于查看是否完整。然后再放入da数组
		ccd++;
		if(ccd==1)//已经运营了++
		{
			TR0=1;  //开始计数
     	ET0 = 1;//允许中断
	  }
    if(ccd==89)
		{
			ccd=0;
			ReFlag=1; //标志位  数据接收成功
		}
  }
  if((S3CON & 2) != 0)
	{
		  unsigned char k=0;
			cuun++;
		
		  dack[0]=cuun;
			GetCRC(dack,CRC);   //串口发送指令  添加crc
			dack[6]=CRC[1];
			dack[7]=CRC[0];
			
			if(cuun==MAXDev)
				  cuun=0;
		
      S3CON &= ~2;                        //清除标志位
			P5_4=1;                            //半双工发送状态
			for(k=0;k<8;k++)
			{
				 S3BUF=dack[k];               	//发送  二进制转换为十进制
         while(!((S3CON & 2) != 0));
				 S3CON &= ~2;                     //清除标志位
  	  }
			
			//延时5ms
			Delay50us();
		  P5_4=0;//半双工接收状态
			T4T3M |= 0x80;	
			IE2 |= 0x40;       //开启
  }
}


/*******************************************************************************************
*   4  定时器4开始工作用于发送串口数据  70ms --> 2s
****************************************************************************************/

void Time4() interrupt TIMER4_VECTOR 
{
	RestartFlag++;
	if(RestartFlag>1714)//十分钟-8570   30分钟-25710
  {
			 RestartFlag=0;
       IAP_CONTR = 0x60;
  }

	ckcount++;
	if(ckcount==Ticks)
	{
		  ckcount=0;
			T4T3M &= ~0x80;	
	    IE2 &= ~0x40;      //关闭定时器
			S3CON |= 0x02; //中断发送我们电表数据
  }
}


/*******************************************************************************************
*   0  定时器0开始工作 中断时间  5.9ms
****************************************************************************************/

void Time0() interrupt TIMER0_VECTOR 
{
	
	T0Flag++;
	if(T0Flag==2)
	{
		unsigned char zero=0;//清零数组长度

		T0Flag=0;
	
		TR0=0;  //停止计数  计数结束
		ET0 = 0;//不允许中断
		if(ReFlag==1)//证明我们的接收是正确的89位数据
		{
				char jz=0;
				jz=dare[zero];
				for(zero=0;zero<89;zero++)//清空数据数组89字节
				{
					Maxdata[jz-1][zero]=dare[zero];
		      SendOrNot_Flag[jz-1]=0;//标志那个不发算

					dare[zero]=0x00;
				}
			//	GPRS_Flag[jz-1][0]=0;
			//	Check_Mas(jz);//检查是哪一个设备出现了问题
			
			//循环检测数据异常
			 
				
	//			CheckCRC(&Maxdata[jz][0],CRC);//校验
	//			GETDATA(&Maxdata[jz][0]);//将数据解析后放入我们的program数组里面
		}
		else            //理论上出现错误应该全部清零的
		{
			ccd=0;//串口接收清零计数
		}
		ReFlag=0;  //已经进入中断函数
	}
}

/***********************************************************

检测时哪些设备出现问题  参数为传入的设备号

*/
void Check_Mas(unsigned char sbid)
{
		 char i=0;
    unsigned char cg=0;
		if(onescheck==1)//第一次传入调用
		{
			onescheck=0;
			sbnumb=sbid;
		}
	 if(sbnumb!=sbid)  //出现问题
	 {
		
			if(sbnumb>sbid)  
			{
				for(cg=sbnumb;cg<6;cg++)
				{
						 Maxdata[cg-1][87]=0x00;
						 Maxdata[cg-1][88]=0x00;
						 SendOrNot_Flag[cg-1]=1;//标志那个不发算
						 GPRS_Flag[cg-1][1]+=1;//二维数组第二位为数据位，第一位为标志位
				}
			//	cg=0;
				
				for(cg=1;cg<sbid;cg++)
				{
						 Maxdata[cg-1][87]=0x00;
						 Maxdata[cg-1][88]=0x00;
					   SendOrNot_Flag[cg-1]=1;//标志那个不发算
						 GPRS_Flag[cg-1][1]+=1;
				}
				
			}
			else
			{
					for(cg=sbnumb;cg<sbid;cg++)
					{
							 Maxdata[cg-1][87]=0x00;
							 Maxdata[cg-1][88]=0x00;
						   SendOrNot_Flag[cg-1]=1;//标志那个不发算
							 
							 GPRS_Flag[cg-1][1]+=1;

					}
			 }
			sbnumb=sbid;
	 }
	 
	 for(i=0;i<3;i++)
	 {
			 if((GPRS_Flag[i][0]==0)&&(GPRS_Flag[i][1]>=3))
				{
					 write_GPRS(i+1+48);
					 
					 GPRS_Flag[0][1]=0;
					 GPRS_Flag[1][1]=0;
					 GPRS_Flag[2][1]=0;
					 
					 GPRS_Flag[i][0]=1;//已经发送过的东西
				}
   	}

		sbnumb++;
		if(sbnumb==6)
			{sbnumb=1;}

}

void InitGPRS()  //初始化语句
{
	unsigned char i=0;
	unsigned char s1[5]={"+++\r\n"};
  unsigned char s2[3]={"A\r\n"};
  for(i=0;i<5;i++)          //发送10位数据位  发送至PC端
	 {
				SBUF=s1[i];
				while(!TI);
				TI=0;
	 }
	//Delay1000ms();
	Delay1000ms();

	for(i=0;i<3;i++)          //发送10位数据位  发送至PC端
	{
				SBUF=s2[i];
				while(!TI);
				TI=0;
	}
	//	Delay1000ms();

	  //write_GPRS();
}

void write_GPRS(unsigned char a)   //向GPRS发短信
{
	unsigned char i=0;
	unsigned char s5[49]={"AT+CISMSSEND=15764271072,3,你好, 号设备出现异常\r\n"};
 
  s5[32]=a;
	//打电话
	for(i=0;i<67;i++)          //发送10位数据位  发送至PC端
	{
			SBUF=s5[i];
			while(!TI);
			TI=0;
	}
}


void send_GPRS()   //向GPRS发送数据
{
	unsigned char i=0;
	unsigned char c4[17]={"ATD15764271072;\r\n"};  //打电话
	unsigned char s4[5]={"ATH\r\n"};              //挂断电话
	unsigned char s5[49]={"AT+CISMSSEND=15764271072,3,你好, 号设备出现异常\r\n"};
 
  //   s5[32]=a;
	              //打电话
		for(i=0;i<17;i++)          //发送10位数据位  发送至PC端
		 {
				SBUF=c4[i];
				while(!TI);
				TI=0;
		 }
}

void Delay1000ms()  //@11.0592MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 43;
	j = 6;
	k = 203;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}