#include "STC15FXXXX.H"
#include"PUBLIC.H"

/*数据位的位置*/

unsigned char Head=0;
unsigned char RoomID=1;
unsigned char DEviceID=2;
unsigned char Operate=3;
unsigned char Dateh=4;
unsigned char Datel=5;
unsigned char Dated=6 ;
unsigned char Save =7;
unsigned char Check=8;
unsigned char Teal =9;


unsigned char FLAG_Recd=0;  //用于查看是哪一个促发了中断
unsigned char FLAG_Send=0;  //用于查看是哪一个促发了中断
unsigned char BUSY=0;  //查看当前的哪一个促发啦

unsigned char DATA_PUBLIC[10]={ 0X7E,0X10,0X01,0X00,0X00,0X00,0X00,0X08,0X00,0X7E};

//unsigned char DATA_Serial1[10]={0X7E,0X10,0X01,0X00,0X00,0X00,0X00,0X08,0X00,0X7E};
unsigned char DATA_Serial2[10]={0X7E,0X10,0X01,0X00,0X00,0X00,0X00,0X08,0X00,0X7E};
unsigned char DATA_Serial3[10]={0X7E,0X10,0X01,0X00,0X00,0X00,0X00,0X08,0X00,0X7E};
unsigned char DATA_Serial4[10]={0X7E,0X10,0X01,0X00,0X00,0X00,0X00,0X08,0X00,0X7E};


/******************************************
*     十进制转换为十六进制                
******************************************

unsigned char dec_to_hex(uchar b)
{
	uchar hex_buf=0;
	uchar i=0; 
	uchar arr[2]={0,0};
 if(b>15)
	{   
		  i=1;//标志着大于15的两位数
			arr[0]=b%16;
			b=b/16;
			arr[1]=b%16;
	}
 else
 {
	 i=0;
	 arr[0]=b%16;
 }
 
	if(i==1)       
	{
		switch(arr[1])   
		{   
				 case 10: arr[i]=0X0A;	break;   
				 case 11: arr[i]=0X0B;	break;   
				 case 12: arr[i]=0X0C;  break;  
				 case 13: arr[i]=0X0D;	break; 
				 case 14: arr[i]=0X0E;	break;  
				 case 15: arr[i]=0X0F;	break;  
				 default: 	break; 
  	}  
	}
	if(i==0)
	{
		switch(arr[0])   
		{   
				 case 10: arr[i]=0X0A;	break;   
				 case 11: arr[i]=0X0B;	break;   
				 case 12: arr[i]=0X0C;  break;  
				 case 13: arr[i]=0X0D;	break; 
				 case 14: arr[i]=0X0E;	break;  
				 case 15: arr[i]=0X0F;	break;  
				 default: 	break; 
  	}  
	}
	
	hex_buf=((arr[1] << 4) | arr[0]);
	return hex_buf;
}*/



/******************************************
*      十六进制转换为十进制                 
*****************************************
unsigned char hex_to_dec(uchar a)
{
	  uchar dec_buf=0;
 	  uchar str[3]={0,0,0};
		uchar j=0;
		while(a)
		{
				str[j]=a%10;                                        
				a/=10;    
				j++;
		}
		dec_buf=str[2]*100+str[1]*10+str[0];
		return dec_buf;
}
**/