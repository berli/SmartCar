#include	"STC15Fxxxx.H"
#include  "RFIDSERIAL.h"  //����Э��
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



/*************	���س�������	**************/
#define MAIN_Fosc		11059200L	//������ʱ��
#define	RX1_Lenth		32			//���ڽ��ջ��峤��
#define	BaudRate1		115200UL	//ѡ������

#define	Timer1_Reload	(65536UL -(MAIN_Fosc / 4 / BaudRate1))		//Timer 1 ��װֵ�� ��Ӧ300KHZ
#define	Timer2_Reload	(65536UL -(MAIN_Fosc / 4 / BaudRate1))		//Timer 2 ��װֵ�� ��Ӧ300KHZ


uchar all=0;           
uchar go=0;            //10λ����
uchar Checked=0;       //���ڱ�־�Ƿ����������

uchar DATA_SEND[10]={ 0x7E, 0x00,     0,  0,      0,0,0,     0x08,      0,       0x7E};
uchar DATA_GET[10]= { 0x7E, 0x00,     0,  0,      0,0,0,     0x08,      0,       0x7E};

/*********************************************************************
                *�����������Ա��Լ�����
**********************************************************************/

void UartInit1(void);
void UartInit2(void);
void UartInit3(void);
void UartInit4(void);

void Send_Data();
void Get_Data_ONE();
void Get_Data_TWO();  //�ڶ����� ���յ� ��һ¥����
void Get_Data_THREE();
void Get_Data_FOUR();  

//void Must_Modify();
//void CHECK_SUM();
void Receive_Data();
void CHECK_FUN();


/*******************************************************************************************
*     ���ڷ���ǰ ���¸�ֵ��������� ����������
****************************************************************************************/
/*
void Must_Modify()
{
	DATA_SEND[0]=DATA_PUBLIC[Head];     //����ͷ
	DATA_SEND[1]=DATA_PUBLIC[RoomID];  //�żٺ�
	DATA_SEND[2]=DATA_PUBLIC[DEviceID];//�豸��
	DATA_SEND[3]=DATA_PUBLIC[Operate]; //����
	DATA_SEND[4]=DATA_PUBLIC[Dateh];   //��������λ
	DATA_SEND[5]=DATA_PUBLIC[Datel];  //��������λ
	DATA_SEND[6]=DATA_PUBLIC[Dated];   //С����λ
	DATA_SEND[7]=DATA_PUBLIC[Save];    //����
	DATA_SEND[9]=DATA_PUBLIC[Teal]; //����β
}

*/
/*******************************************************************************
          �жϷ��ͺ���
********************************************************************************/

void Send_Data()
{
	 uint i=0;
	 if(FLAG_Recd==2)
	 {
		  for(i=0;i<10;i++)          //����10λ����λ  ������PC��
			 {
					SBUF=DATA_Serial2[i];
					while(!TI);
					TI=0;
			 }
   }
	 else if(FLAG_Recd==3)
	 {
		  for(i=0;i<10;i++)          //����10λ����λ  ������PC��
			 {
					SBUF=DATA_Serial3[i];
					while(!TI);
					TI=0;
			 }
   }
	 else if(FLAG_Recd==4)
	 {
		  for(i=0;i<10;i++)          //����10λ����λ  ������PC��
			 {
					SBUF=DATA_Serial4[i];
					while(!TI);
					TI=0;
			 }
   }
	 
	 FLAG_Recd=0;
}


/*******************************************************************************
          �жϽ������ݺ���
********************************************************************************/

void Get_Data_ONE()//���յ�������  �ֽ׶�ֻ��ͷβ �Ͳ�����  ������û��У�� 
{
	 Receive_Data();
	 CHECK_FUN();
}


//�ڶ�����  ���յ�������

void Get_Data_TWO()  
{
	if((all==0)&&(S2BUF==0x7E))// �жϵ�һ���ǲ���0x7e  ��ȷ���ǲ���β��7e
	   {go=1;}
	if((go==1)&&(all==1)&&(S2BUF==0X7E))//�ڶ���7e
	   {all=0;}                        //ֻ��all���㣬��������ڶ�������
	if(go==1)  //10���ַ�����������
	{
		DATA_Serial2[all]=S2BUF;
	 	all++;
		if(all==10)
		{
			go=0;         //���Խ��ܵ�һ���ַ�
			all=0;
			FLAG_Recd=2;//��־Ϊ  ����2
			TI=1;
			//Checked=1;  //������Ҫ�� һ�����ݰ��������
	  }
  }
}


void Get_Data_THREE()  
{
	if((all==0)&&(S3BUF==0x7E))// �жϵ�һ���ǲ���0x7e  ��ȷ���ǲ���β��7e
	   {go=1;}
	if((go==1)&&(all==1)&&(S3BUF==0X7E))//�ڶ���7e
	   {all=0;}                        //ֻ��all���㣬��������ڶ�������
	if(go==1)  //10���ַ�����������
	{
		DATA_Serial3[all]=S3BUF;
	 	all++;
		if(all==10)
		{
			go=0;         //���Խ��ܵ�һ���ַ�
			all=0;
			FLAG_Recd=3;//��־Ϊ  ����2
			TI=1;
			//Checked=1;  //������Ҫ�� һ�����ݰ��������
	  }
  }
}


void Get_Data_FOUR()  
{
	if((all==0)&&(S4BUF==0x7E))// �жϵ�һ���ǲ���0x7e  ��ȷ���ǲ���β��7e
	   {go=1;}
	if((go==1)&&(all==1)&&(S4BUF==0X7E))//�ڶ���7e
	   {all=0;}                        //ֻ��all���㣬��������ڶ�������
	if(go==1)  //10���ַ�����������
	{
		DATA_Serial4[all]=S4BUF;
	 	all++;
		if(all==10)
		{
			go=0;         //���Խ��ܵ�һ���ַ�
			all=0;
			FLAG_Recd=4;//��־Ϊ  ����2
			TI=1;
			//Checked=1;  //������Ҫ�� һ�����ݰ��������
	  }
  }
}
/***************************************************************
             ����һλһλ�Ľ�������
*******************************************************************/

void Receive_Data()//���յ�������  �ֽ׶�ֻ��ͷβ �Ͳ�����  ������û��У�� 
{
	if((all==0)&&(SBUF==0x7E))// �жϵ�һ���ǲ���0x7e  ��ȷ���ǲ���β��7e
	   {go=1;}
	if((go==1)&&(all==1)&&(SBUF==0X7E))//�ڶ���7e
	   {all=0;}                        //ֻ��all���㣬��������ڶ�������
	if(go==1)  //10���ַ�����������
	{
		DATA_GET[all]=SBUF;
	 	all++;
		if(all==10)
		{
			go=0;         //���Խ��ܵ�һ���ַ�
			all=0;
			Checked=1;  //������Ҫ�� һ�����ݰ��������
	  }
  }
}


/***************************************************************
              ����У�� �鿴����
*******************************************************************/

void CHECK_FUN()
{
	if(Checked==1)//���ݽ������
	{
		Checked=0;
		if((DATA_GET[0]==0X7E)&&(DATA_GET[9]==0X7E))//�Ƿ��ǶԵ�  &&(DATA_GET[1]==0x10)
		{
			switch(DATA_GET[1])   //�ſ�ָ��
			{
				case 0X10:   S2CON |= S2TI;  break; //���͸�һ¥ 0x10
				case 0X20:   S3CON |= 2;     break; //���͸�һ2¥ 0x20
				case 0X30:   S4CON |= S4TI;  break; //���͸�3һ¥ 0x30
				default:      break;
			}
		}
		else
		{
			char k=0;
			for(k=0;k<10;k++)
			   { DATA_GET[k]=0;}//ָ����� �������
    }
	}
}


/***************************************************************
              ��У���   ���ڷ���    ����ȥ������ȫ��У��
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
				check_sum+=DATA_SEND[j];           //���ִ��ڵ���0x30�ļ�ȥ0x30 �����
				check_sum-=0x30;
			}
			else
				check_sum+=DATA_SEND[j];	
  }
	check_sum=~check_sum+1;           //У���ȡ����1
	DATA_SEND[8]=check_sum;  //У��λ
}

*/

/*************************************************************  
	   1   *��ʼ�������ж�
**********************************************************/

void UartInit1(void)             
{
    ACC = P_SW1;
    ACC &= ~(1 | S1_S1);    //S1_S0=0 S1_S1=0
    P_SW1 = ACC;                //(P3.0/RxD, P3.1/TxD) �л�Ϊ(P3.6/RxD_2, P3.7/TxD_2)
    SCON = 0x50;         
    T2L = (65536 - (MAIN_Fosc/4/BaudRate1));   //���ò�������װֵ
    T2H = (65536 - (MAIN_Fosc/4/BaudRate1))>>8;
    AUXR = 0x14;                //T2Ϊ1Tģʽ, ��������ʱ��2
    AUXR |= 0x01;               //ѡ��ʱ��2Ϊ����1�Ĳ����ʷ�����
    ES = 1;                     //ʹ�ܴ���1�ж�
    EA = 1;
}

void UartInit2(void)             
{
	  P_SW2 &= ~S2_S0;            //S2_S0=0 (P1.0/RxD2, P1.1/TxD2)
    S2CON = 0x50;               //8λ�ɱ䲨����
    T2L = (65536 - (MAIN_Fosc/4/BaudRate1));   //���ò�������װֵ
    T2H = (65536 - (MAIN_Fosc/4/BaudRate1))>>8;
    AUXR = 0x14;                //T2Ϊ1Tģʽ, ��������ʱ��2
    IE2 |= 0x01;                 //ʹ�ܴ���2�ж�
    EA = 1;
}
//						          7      6      5      4      3      2     1     0        Reset Value
//sfr S2CON = 0x9A;		S2SM0    -    S2SM2  S2REN  S2TB8  S2RB8  S2TI  S2RI      00000000B		 //S2 Control

void UartInit3(void)             
{
	S3CON &= ~(1<<7);	//8λ����
	S3CON &= 0xBF;		//ѡ��ʱ��2
	S3_USE_P00P01();		//UART3 ʹ��P0.0 P0.1��	Ĭ��
	AUXR = 0x14;                //T2Ϊ1Tģʽ, ��������ʱ��2
  TH2 = (u8)(Timer2_Reload >> 8);
	TL2 = (u8)Timer2_Reload;
	IE2  |= (1<<3);		//�����ж�es3
	S3CON |=  (1<<4);	//�������
	AUXR |=  (1<<4);	//Timer run enable
	EA = 1;		//����ȫ���ж�
}

void UartInit4(void)             
{
		P_SW2 &= ~S4_S0;            //S4_S0=0 (P0.2/RxD4, P0.3/TxD4)
    S4CON = 0x10;               //8λ�ɱ䲨����
    T2L = (65536 - (MAIN_Fosc/4/BaudRate1));   //���ò�������װֵ
    T2H = (65536 - (MAIN_Fosc/4/BaudRate1))>>8;
    AUXR = 0x14;                //T2Ϊ1Tģʽ, ��������ʱ��2
    IE2 |= 0x10;                 //ʹ�ܴ���4�ж�
    EA = 1;
}

/*******************************************************************************************
*   1   �����жϺ��� ���ͺͽ������ݰ�
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
        TI = 0;                 //���TIλ
				Send_Data();
    }
}


/*******************************************************************************************
*   2 �����жϺ��� ���ͺͽ������ݰ�
****************************************************************************************/

void Package_Send_AND_Get2 (void) interrupt UART2_VECTOR
{
	  char k=0;
    if (S2CON & S2RI)
    {
        S2CON &= ~S2RI;         //���S2RIλ
				Get_Data_TWO();
    }
    if (S2CON & S2TI)
    {
        S2CON &= ~S2TI;         //���S2TIλ
				for(k=0;k<10;k++)
				{
						S2BUF = DATA_GET[k];               
						while(!(S2CON & S2TI));
						S2CON &= ~S2TI;         //���S2TIλ
        }
   }
}


/*******************************************************************************************
*      3 �����жϺ��� ���ͺͽ������ݰ�
****************************************************************************************/

void Package_Send_AND_Get3 (void) interrupt UART3_VECTOR
{
	char k=0;
  if((S3CON & 1) != 0)
	{
		S3CON &= ~1;                       //�����־λ
    Get_Data_THREE();
  	}
  if((S3CON & 2) != 0)
	{
			S3CON &= ~2;                        //�����־λ
			for(k=0;k<10;k++)
			{
					S3BUF=DATA_GET[k];      //dec_to_hex(RX3_Buffer);		//����  ������ת��Ϊʮ����
					while(!((S3CON & 2) != 0));
					S3CON &= ~2;                     //�����־λ
  	  }
   }
	
}


void Package_Send_AND_Get4 (void) interrupt UART4_VECTOR
{
  	char k=0;
	 if (S4CON & S4RI)
    {
        S4CON &= ~S4RI;         //���S4RIλ
        Get_Data_FOUR(); 
    }
    if (S4CON & S4TI)
    {
        S4CON &= ~S4TI;         //���S4TIλ
				for(k=0;k<10;k++)
				{
						S4BUF=DATA_GET[k];            
						while(!(S4CON & S4TI));
						S4CON &= ~S4TI;         //���S4TIλ
				}
    }
}

