#include "example0.h"
#include "uip.h"
//#include "AT45DB321.h"
#include "USART.h"
#include "stdio.h"
#include <string.h>

code unsigned char Recmd[16]={0X00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F};
void example0_init(void) 
{
	uip_listen(HTONS(8000));
	P0_2 =0;  //������  Ĭ�Ϲر�
	P0_3 =0;  //��      Ĭ�Ϲر�
	P5_4=0;//��˫������״̬
	//das=MAXDev;//������������
}

/********************** ***********************************
                   У�����ְ
  ************************************************************/
unsigned char CheckSum()
{
	unsigned char sum=0;
	sum=uip_appdata[0]+uip_appdata[1]+uip_appdata[2]-0x01;
	return sum;
}

void example0_app(void) 
{
  if(uip_acked())
	{
	//	SendString("example0 is acked!\r\n");
	}
	if(uip_closed())
	{
	//	SendString("example0 is closed!\r\n");
	}
	if(uip_connected())
	{
	//	SendString("example0 is connected!\r\n");
	}
	
	if(uip_newdata() || uip_rexmit()) 
	{
	//	SendString("example0 is uip_newdata!\r\n");		
	
		if ((uip_len == 4)&&(uip_appdata[3]==CheckSum()))  //���չ��������ݳ���4   ��� ����1 ����2 У��λ
		{
			if(uip_appdata[0] == 0x01)  //1���豻
			{
					if((uip_appdata[1] == 0x01)&&(uip_appdata[2] == 0x01))//�򿪷�����
					{
						P0_2 =1;
					//	uip_send(&Recmd[1], 1);
					}
					else if((uip_appdata[1] == 0x01)&&(uip_appdata[2] == 0x02))//�رշ�����
					{
						P0_2 =0;
					//	uip_send(&Recmd[1], 1);
					}
					else //������������ û�н���
			    {
					//	uip_send(&Recmd[3], 1);
		      }
	  	}
			else if(uip_appdata[0] == 0x02) //�ƹ�
			{
					if((uip_appdata[1] == 0x01)&&(uip_appdata[2] == 0x01))//�򿪵�
					{
						P0_3 =1;
					//	uip_send(&Recmd[1], 1);
					}
					else if((uip_appdata[1] == 0x01)&&(uip_appdata[2] == 0x02))//�رյ�
					{
						P0_3 =0;
					//	uip_send(&Recmd[1], 1);
					}
					else //������������ û�н���
			    {
					//	uip_send(&Recmd[3], 1);
		      }
		  }
			else if(uip_appdata[0] == 0x03) //�������ǵĻ�ȡ����
			{
					if((uip_appdata[1] == 0x01)&&(uip_appdata[2] <= MAXDev))//��ȡ����
					{
						unsigned char zero=0,jz=0;
						
						jz=uip_appdata[2];       //���ڼ�ס����һ�������ֵ
						
						if(SendOrNot_Flag[jz-1]==0)
						{
								das=&Maxdata[jz-1][0];//������������
								
								memcpy(&uip_appdata[0],das,89);  //���Ʒ���ֵ
								uip_send(&uip_appdata[0], 89);   //���͸��Ƶ�ֵ
						}
					//�������ˣ����ǵ��������豸�ڴ˶�ȡʱ����Ϊ���ˡ����Բ������
					
					
//						for(zero=0;zero<89;zero++)//�����������49�ֽ�
//						{
//					        Maxdata[jz-1][zero]=0x00;
//					  }

					}
					else //������������ û�н���
			    {
					//	uip_send(&Recmd[3], 1);
		      }
		  }
			
			else if((uip_appdata[0] == 0x11)&&(uip_appdata[1] == 0x01)) //����
			{
				if(uip_appdata[2] == 0x01)
				{IAP_CONTR = 0x20;  }         //�����λ,ϵͳ���´��û���������ʼ���г���
        else if(uip_appdata[2] == 0x02)
				{	IAP_CONTR = 0x60;  }         //�����λ,ϵͳ���´�ISP��������ʼ���г���
			  else
				{
			   // 	uip_send(&Recmd[5], 1);
			  }
		  }
			else //������������ û�н���
			{
			//		uip_send(&Recmd[6], 1);
		
		  }
		}
		else
		{
		//	uip_send(&Recmd[7], 1);
	  }
	}
}