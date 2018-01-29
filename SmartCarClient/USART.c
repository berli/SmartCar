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

#define FOSC 11059200L          //ϵͳƵ��
#define BAUD 115200             //���ڲ�����

#define NONE_PARITY     0       //��У��

#define PARITYBIT EVEN_PARITY   //����У��λ

#define S3RI  0x01              //S3CON.0
#define S3TI  0x02              //S3CON.1
#define S3RB8 0x04              //S3CON.2
#define S3TB8 0x08              //S3CON.3

#define S3_S0 0x02              //P_SW2.1


static unsigned int ccd=0;        //���ڼ��������յ���ϴ������ݶ��ٸ� Ĭ��49
static unsigned char ckcount=0; //���ڼ���28�ξ�2s;
static unsigned char cuun=0;
static unsigned char onescheck=1;
static unsigned char sbnumb=0;

static unsigned int RestartFlag=0;

unsigned char *das;
unsigned char ReFlag=0;
unsigned char T0Flag=0;

////da���ڷ��ͳ�ȥ�ĺ��� ��������
//unsigned char data da[49]={0x03, 0x03 , 0x2C , 0x00 , 0x00 , 0x00 , 0x00 , 0x08 , 0xFE , 0x00 , 0x00 ,
//                           0x08 , 0xFE, 0x08 , 0xFE , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 ,
//                           0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 ,
//                           0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00,
//                           0x00 , 0x00 , 0x00 , 0xC3 , 0x02};
//��ά���鱣�洫������  60̨�豸 ���ֵ
//extern unsigned char Maxdata[MAXDev][49]={0x00};


//��ȡ����ָ��   �Զ���ѭ������
unsigned char data dack[8]={0x00,0x03,0x00,0x17,0X00,0x2A,0x00,0x00};


unsigned char data dare[89]={0x00};//��������485����������
unsigned char data CRC[2]={0x00};//�������У���
unsigned int Program[MAXDev][0x09]={0};//���ǵĶ�ά���鱣�������������ݣ��������
unsigned char Maxdata[MAXDev][89]={0x00};//�����ά��������

unsigned char GPRS_Flag[5][2]={0x00};//��ά���鱣��GPRS�ı�־λ
unsigned char SendOrNot_Flag[5]={0x01,0x01,0x01,0x01,0x01};//��־λ���ǵ�ѡ���Ƿ���




/*****************************************************************
    ��������
****************************************************************/
void CheckCRC(unsigned char *message,unsigned char *CRC);
void GETDATA(unsigned char *values);
void GetCRC(unsigned char message[],unsigned char *CRC);
void Delay50us();		//@11.0592MHz
void Check_Mas(unsigned char sbid);

void InitGPRS();  //��ʼ�����

void Delay1000ms();  //@11.0592MHz

void send_GPRS();//��GPRS��������
void write_GPRS(unsigned char a);   //��GPRS������

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


//���͵�ʱ��crcУ��ȡֵ��һά����

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


//���ڳ�ʼ��1
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
	REN = 1;	//�������
	PS=1;         //�߼��ж�����
  ES = 1;				//�������ж�
	EA = 1;				//�����ж�
}

void USART3_Init(void)		//9600bps@11.0592MHz  ����2��485ͨ�š�modbusͨ��Э��
{
	S3CON &= ~(1<<7);	//8λ����
	S3CON &= 0xBF;		//ѡ��ʱ��2
	S3_USE_P00P01();		//UART3 ʹ��P0.0 P0.1��	Ĭ��
	AUXR |= 0x14;                //T2Ϊ1Tģʽ, ��������ʱ��2
  TH2 = (u8)((65536UL -(11059200UL / 4 / 9600L)) >> 8);
	TL2 = (u8)(65536UL -(11059200UL / 4 / 9600L));
	IE2  |= (1<<3);		//�����ж�es3
	S3CON |=  (1<<4);	//�������
	AUXR |=  (1<<4);	//Timer run enable
	EA = 1;		//����ȫ���ж�
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
	ET0 = 0;//�������ж�
  EA=1;	
}


//����һ���ַ�,ASCII��
void SendASC(unsigned char d)
{
	SBUF=d;
	while(!TI);	//�ȴ��������,TI��λ
	TI=0;
}

//�����ַ���
void SendString(unsigned char *str)	
{
	while(*str)
	{
		SendASC(*str) ;
		str++;
	}
}


//�����жϷ������
void USART_Interrupt(void)  interrupt UART1_VECTOR 	  //�������� �ͷ���main����������һ����
{
//	unsigned char da,db;
	//EA=0; 
  if(RI) 			//�����ж�RI(�����жϱ�־)�Ƿ�Ϊ1
  {
    RI=0;
		//�������ڷ���GPRS�绰�Ͷ���
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
UART3 �жϷ������
-----------------------------*/

void Package_Send_AND_Get3 (void) interrupt UART3_VECTOR
{
  if((S3CON & 1) != 0)
	{
		S3CON &= ~1;                       //�����־λ
		dare[ccd]=S3BUF;                   //���յ������ݷ���dare�����ڲ鿴�Ƿ�������Ȼ���ٷ���da����
		ccd++;
		if(ccd==1)//�Ѿ���Ӫ��++
		{
			TR0=1;  //��ʼ����
     	ET0 = 1;//�����ж�
	  }
    if(ccd==89)
		{
			ccd=0;
			ReFlag=1; //��־λ  ���ݽ��ճɹ�
		}
  }
  if((S3CON & 2) != 0)
	{
		  unsigned char k=0;
			cuun++;
		
		  dack[0]=cuun;
			GetCRC(dack,CRC);   //���ڷ���ָ��  ���crc
			dack[6]=CRC[1];
			dack[7]=CRC[0];
			
			if(cuun==MAXDev)
				  cuun=0;
		
      S3CON &= ~2;                        //�����־λ
			P5_4=1;                            //��˫������״̬
			for(k=0;k<8;k++)
			{
				 S3BUF=dack[k];               	//����  ������ת��Ϊʮ����
         while(!((S3CON & 2) != 0));
				 S3CON &= ~2;                     //�����־λ
  	  }
			
			//��ʱ5ms
			Delay50us();
		  P5_4=0;//��˫������״̬
			T4T3M |= 0x80;	
			IE2 |= 0x40;       //����
  }
}


/*******************************************************************************************
*   4  ��ʱ��4��ʼ�������ڷ��ʹ�������  70ms --> 2s
****************************************************************************************/

void Time4() interrupt TIMER4_VECTOR 
{
	RestartFlag++;
	if(RestartFlag>1714)//ʮ����-8570   30����-25710
  {
			 RestartFlag=0;
       IAP_CONTR = 0x60;
  }

	ckcount++;
	if(ckcount==Ticks)
	{
		  ckcount=0;
			T4T3M &= ~0x80;	
	    IE2 &= ~0x40;      //�رն�ʱ��
			S3CON |= 0x02; //�жϷ������ǵ������
  }
}


/*******************************************************************************************
*   0  ��ʱ��0��ʼ���� �ж�ʱ��  5.9ms
****************************************************************************************/

void Time0() interrupt TIMER0_VECTOR 
{
	
	T0Flag++;
	if(T0Flag==2)
	{
		unsigned char zero=0;//�������鳤��

		T0Flag=0;
	
		TR0=0;  //ֹͣ����  ��������
		ET0 = 0;//�������ж�
		if(ReFlag==1)//֤�����ǵĽ�������ȷ��89λ����
		{
				char jz=0;
				jz=dare[zero];
				for(zero=0;zero<89;zero++)//�����������89�ֽ�
				{
					Maxdata[jz-1][zero]=dare[zero];
		      SendOrNot_Flag[jz-1]=0;//��־�Ǹ�������

					dare[zero]=0x00;
				}
			//	GPRS_Flag[jz-1][0]=0;
			//	Check_Mas(jz);//�������һ���豸����������
			
			//ѭ����������쳣
			 
				
	//			CheckCRC(&Maxdata[jz][0],CRC);//У��
	//			GETDATA(&Maxdata[jz][0]);//�����ݽ�����������ǵ�program��������
		}
		else            //�����ϳ��ִ���Ӧ��ȫ�������
		{
			ccd=0;//���ڽ����������
		}
		ReFlag=0;  //�Ѿ������жϺ���
	}
}

/***********************************************************

���ʱ��Щ�豸��������  ����Ϊ������豸��

*/
void Check_Mas(unsigned char sbid)
{
		 char i=0;
    unsigned char cg=0;
		if(onescheck==1)//��һ�δ������
		{
			onescheck=0;
			sbnumb=sbid;
		}
	 if(sbnumb!=sbid)  //��������
	 {
		
			if(sbnumb>sbid)  
			{
				for(cg=sbnumb;cg<6;cg++)
				{
						 Maxdata[cg-1][87]=0x00;
						 Maxdata[cg-1][88]=0x00;
						 SendOrNot_Flag[cg-1]=1;//��־�Ǹ�������
						 GPRS_Flag[cg-1][1]+=1;//��ά����ڶ�λΪ����λ����һλΪ��־λ
				}
			//	cg=0;
				
				for(cg=1;cg<sbid;cg++)
				{
						 Maxdata[cg-1][87]=0x00;
						 Maxdata[cg-1][88]=0x00;
					   SendOrNot_Flag[cg-1]=1;//��־�Ǹ�������
						 GPRS_Flag[cg-1][1]+=1;
				}
				
			}
			else
			{
					for(cg=sbnumb;cg<sbid;cg++)
					{
							 Maxdata[cg-1][87]=0x00;
							 Maxdata[cg-1][88]=0x00;
						   SendOrNot_Flag[cg-1]=1;//��־�Ǹ�������
							 
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
					 
					 GPRS_Flag[i][0]=1;//�Ѿ����͹��Ķ���
				}
   	}

		sbnumb++;
		if(sbnumb==6)
			{sbnumb=1;}

}

void InitGPRS()  //��ʼ�����
{
	unsigned char i=0;
	unsigned char s1[5]={"+++\r\n"};
  unsigned char s2[3]={"A\r\n"};
  for(i=0;i<5;i++)          //����10λ����λ  ������PC��
	 {
				SBUF=s1[i];
				while(!TI);
				TI=0;
	 }
	//Delay1000ms();
	Delay1000ms();

	for(i=0;i<3;i++)          //����10λ����λ  ������PC��
	{
				SBUF=s2[i];
				while(!TI);
				TI=0;
	}
	//	Delay1000ms();

	  //write_GPRS();
}

void write_GPRS(unsigned char a)   //��GPRS������
{
	unsigned char i=0;
	unsigned char s5[49]={"AT+CISMSSEND=15764271072,3,���, ���豸�����쳣\r\n"};
 
  s5[32]=a;
	//��绰
	for(i=0;i<67;i++)          //����10λ����λ  ������PC��
	{
			SBUF=s5[i];
			while(!TI);
			TI=0;
	}
}


void send_GPRS()   //��GPRS��������
{
	unsigned char i=0;
	unsigned char c4[17]={"ATD15764271072;\r\n"};  //��绰
	unsigned char s4[5]={"ATH\r\n"};              //�Ҷϵ绰
	unsigned char s5[49]={"AT+CISMSSEND=15764271072,3,���, ���豸�����쳣\r\n"};
 
  //   s5[32]=a;
	              //��绰
		for(i=0;i<17;i++)          //����10λ����λ  ������PC��
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