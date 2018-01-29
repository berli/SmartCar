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
	P0_2 =0;  //蜂鸣器  默认关闭
	P0_3 =0;  //灯      默认关闭
	P5_4=0;//半双工接收状态
	//das=MAXDev;//关联两个数组
}

/********************** ***********************************
                   校验和求职
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
	
		if ((uip_len == 4)&&(uip_appdata[3]==CheckSum()))  //接收过来的数据长度4   编号 数据1 数据2 校验位
		{
			if(uip_appdata[0] == 0x01)  //1号设被
			{
					if((uip_appdata[1] == 0x01)&&(uip_appdata[2] == 0x01))//打开蜂鸣器
					{
						P0_2 =1;
					//	uip_send(&Recmd[1], 1);
					}
					else if((uip_appdata[1] == 0x01)&&(uip_appdata[2] == 0x02))//关闭蜂鸣器
					{
						P0_2 =0;
					//	uip_send(&Recmd[1], 1);
					}
					else //此命令有问题 没有解析
			    {
					//	uip_send(&Recmd[3], 1);
		      }
	  	}
			else if(uip_appdata[0] == 0x02) //灯光
			{
					if((uip_appdata[1] == 0x01)&&(uip_appdata[2] == 0x01))//打开灯
					{
						P0_3 =1;
					//	uip_send(&Recmd[1], 1);
					}
					else if((uip_appdata[1] == 0x01)&&(uip_appdata[2] == 0x02))//关闭灯
					{
						P0_3 =0;
					//	uip_send(&Recmd[1], 1);
					}
					else //此命令有问题 没有解析
			    {
					//	uip_send(&Recmd[3], 1);
		      }
		  }
			else if(uip_appdata[0] == 0x03) //设置我们的获取数据
			{
					if((uip_appdata[1] == 0x01)&&(uip_appdata[2] <= MAXDev))//获取数据
					{
						unsigned char zero=0,jz=0;
						
						jz=uip_appdata[2];       //用于记住是哪一个数组的值
						
						if(SendOrNot_Flag[jz-1]==0)
						{
								das=&Maxdata[jz-1][0];//关联两个数组
								
								memcpy(&uip_appdata[0],das,89);  //复制发送值
								uip_send(&uip_appdata[0], 89);   //发送复制的值
						}
					//如果清空了，我们的其他的设备在此读取时，就为空了。所以不能清空
					
					
//						for(zero=0;zero<89;zero++)//清空数据数组49字节
//						{
//					        Maxdata[jz-1][zero]=0x00;
//					  }

					}
					else //此命令有问题 没有解析
			    {
					//	uip_send(&Recmd[3], 1);
		      }
		  }
			
			else if((uip_appdata[0] == 0x11)&&(uip_appdata[1] == 0x01)) //重启
			{
				if(uip_appdata[2] == 0x01)
				{IAP_CONTR = 0x20;  }         //软件复位,系统重新从用户代码区开始运行程序
        else if(uip_appdata[2] == 0x02)
				{	IAP_CONTR = 0x60;  }         //软件复位,系统重新从ISP代码区开始运行程序
			  else
				{
			   // 	uip_send(&Recmd[5], 1);
			  }
		  }
			else //此命令有问题 没有解析
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