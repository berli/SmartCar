/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC15F4K60S4 ϵ�� ��ʱ��1��������1�Ĳ����ʷ���������------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966-------------------------*/
/* --- Web: www.STCMCU.com --------------------------------------------*/
/* --- Web: www.GXWMCU.com --------------------------------------------*/
/* ���Ҫ�ڳ�����ʹ�ô˴���,���ڳ�����ע��ʹ����STC�����ϼ�����        */
/* ���Ҫ��������Ӧ�ô˴���,����������ע��ʹ����STC�����ϼ�����        */
/*---------------------------------------------------------------------*/

//��ʾ����Keil������������ѡ��Intel��8058оƬ�ͺŽ��б���
//�����ر�˵��,����Ƶ��һ��Ϊ11.0592MHz


#include "STC15W4K58S4.h"

#include "intrins.h"
#include <string.h>  // �ַ�������ͷ�ļ�

sbit LED = P3 ^ 2;  // ��ӦӲ������
sbit LOUND = P5 ^ 4;  // ��ӦӲ������

typedef unsigned char BYTE;
typedef unsigned int WORD;

BYTE DATA_LENGTH =7;

BYTE DATA_SEND[]={ 0x7E, 0x00,     0,  0,      0,      0,       0x7E};
BYTE DATA_GET[]= { 0x7E, 0x00,     0,  0,      0,      0,       0x7E};

BYTE all=0;           



#define FOSC 11059200L          //ϵͳƵ��
#define BAUD 115200             //���ڲ�����

#define NONE_PARITY     0       //��У��
#define ODD_PARITY      1       //��У��
#define EVEN_PARITY     2       //żУ��
#define MARK_PARITY     3       //���У��
#define SPACE_PARITY    4       //�հ�У��

#define PARITYBIT NONE_PARITY   //����У��λ


#define S1_S0 0x40              //P_SW1.6
#define S1_S1 0x80              //P_SW1.7


bit busy;

void SendData(BYTE dat);
void  SendString(char *s);

void UART_TC (unsigned char *str);
void UART_T (unsigned char UART_data); //���崮�ڷ������ݱ���
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
    SCON = 0x50;                //8λ�ɱ䲨����
#elif (PARITYBIT == ODD_PARITY) || (PARITYBIT == EVEN_PARITY) || (PARITYBIT == MARK_PARITY)
    SCON = 0xda;                //9λ�ɱ䲨����,У��λ��ʼΪ1
#elif (PARITYBIT == SPACE_PARITY)
    SCON = 0xd2;                //9λ�ɱ䲨����,У��λ��ʼΪ0
#endif

    AUXR = 0x40;                //��ʱ��1Ϊ1Tģʽ
    TMOD = 0x00;                //��ʱ��1Ϊģʽ0(16λ�Զ�����)
    TL1 = (65536 - (FOSC/4/BAUD));   //���ò�������װֵ
    TH1 = (65536 - (FOSC/4/BAUD))>>8;
    TR1 = 1;                    //��ʱ��1��ʼ����
    ES = 1;                     //ʹ�ܴ����ж�
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
UART �жϷ������
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
        TI = 0;                 //���TIλ
				busy = 0;               //��æ��־
    }
}

/*----------------------------
���ʹ�������
----------------------------*/
void SendData(BYTE dat)
{
    while (busy);               //�ȴ�ǰ������ݷ������
    ACC = dat;                  //��ȡУ��λP (PSW.0)
    if (P)                      //����P������У��λ
    {
#if (PARITYBIT == ODD_PARITY)
        TB8 = 0;                //����У��λΪ0
#elif (PARITYBIT == EVEN_PARITY)
        TB8 = 1;                //����У��λΪ1
#endif
    }
    else
    {
#if (PARITYBIT == ODD_PARITY)
        TB8 = 1;                //����У��λΪ1
#elif (PARITYBIT == EVEN_PARITY)
        TB8 = 0;                //����У��λΪ0
#endif
    }
    busy = 1;
    SBUF = ACC;                 //д���ݵ�UART���ݼĴ���
}


/*----------------------------
�����ַ���
----------------------------*/

void  SendString(char *s)
{
	 unsigned int i=0;
		 
		   for(i=0;i<DATA_LENGTH;i++)          //��test���� ������PC��
			 {
					SBUF=s[i];
					while(!TI);
					TI=0;
			 }
}

void UART_T (unsigned char UART_data){ //���崮�ڷ������ݱ���
	SBUF = UART_data;	//�����յ����ݷ��ͻ�ȥ
	while(TI == 0);		//��鷢���жϱ�־λ
	TI = 0;			//����жϱ�־λΪ0��������㣩
}

void UART_TC (unsigned char *str){
	while(*str != '\0'){
		UART_T(*str);
		*str++;
	}
	*str = 0;
}


//����  ���յ�������

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


	UART_TC("+++\0"); // �˳�͸��ģʽ
	DELAY_MS( 1500);

	UART_TC("AT\r\n\0");	// ATָ�����
	DELAY_MS(1500);

	//UART_TC("AT+CWSTARTSMART\r\n\0"); // ��ʼ��������ģʽ
	//DELAY_MS(1000);
//	LED = 0; // ����ָʾ������
//	DELAY_MS( 30000); // ���ӳɹ�

	//UART_TC("AT+CWSTOPSMART\r\n\0"); // ������������ģʽ���ͷ�ģ����Դ(����)
 //	DELAY_MS( 1000);
	//LED = 1; // ����ָʾ��Ϩ��

	UART_TC("AT+CIPMUX=0\r\n\0");  // ���õ�����ģʽ
	DELAY_MS(1500);

	UART_TC("AT+CIPSTART=\"TCP\",\"192.168.0.104\",4001\r\n\0");	// ���ӵ�ָ��TCP������
	DELAY_MS( 3500);

	UART_TC("AT+CIPMODE=1\r\n\0"); // ����͸��ģʽ
	DELAY_MS( 1500);

	UART_TC("AT+SAVETRANSLINK=1,\"192.168.0.104\",4001,\"TCP\"\r\n\0"); // ����TCP���ӵ�flash��ʵ���ϵ�͸��
	DELAY_MS(1500);

	UART_TC("AT+CIPSEND\r\n\0");	 // ����͸��ģʽ
	DELAY_MS( 1500);


}
