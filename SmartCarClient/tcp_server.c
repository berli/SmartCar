#include "example0.h"
//#include "example1.h"
#include "tcp_server.h"
//#include "httpd.h"
#include "uip.h"
//#include "mcu_uart.h"


void tcp_server_init(void) 
{
	//httpd_init();
	example0_init();
	//example1_init();
}

void tcp_server_appcall(void) 
{
/*
	u8_t i;
		printuf("\r\nÊÕµ½0x%x ×Ö½Ú\r\n", uip_len);
		for(i=0;i<uip_len;i++)
		{
			printuf("%x ", uip_appdata[i]);
		}
*/
	switch(uip_conn->lport) 
	{
	//	case HTONS(80):
		//	httpd_appcall();break;
		case HTONS(8000):
			example0_app();break;
	//	case HTONS(8001):
	//		example1_app();break;
	}
}
