#include "STC15Fxxxx.H"
#include "RFIDSERIAL.h"
#include "public.h"

sfr IAP_CON=0XC7;

void main(void)
{
	 UartInit4();
	 UartInit3();
   UartInit2();//��ʼ������
	 UartInit1();//��ʼ������
	// Must_Modify();
	
	 while(1)
	 {
		// if(!(P3&0x01))  IAP_CON=0X60;//ռ����Դ����
   }
}
