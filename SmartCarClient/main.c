/*
 * Copyright (c) 2001-2003, Adam Dunkels.
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.  
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
 *
 * This file is part of the uIP TCP/IP stack.
 *
 * $Id: main.c,v 1.10.2.1 2003/10/04 22:54:17 adam Exp $
 *
 */
#include "uip.h" 
#include "uip_arp.h"
//#include "rtl8019as.h"
//#include "httpd.h"
//#include "telnet.h"
#include "enc28j60.h"

#include "tcp_server.h"
#include "USART.h"
#include "stdio.h"
#include "intrins.h"

sfr WDT_CONTR = 0xC1;
sfr S2CON = 0x9A;	//
sfr	S3CON = 0xAC;
#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

#ifndef NULL
#define NULL (void *)0
#endif /* NULL */

/*-----------------------------------------------------------------------------------*/
int main(void)
{
	idata u8_t i, arptimer;
	idata u16_t j;
  
//	USART_Init();
//	 InitGPRS();//初始化GPRS
//	SendString("USART1_Init...\r\n");
	
	
	/* Initialize the device driver. */ 
	//  rtl8019as_init();
	dev_init();
	uip_arp_init();
	//SendString("uip_arp_init...\r\n");
	/* Initialize the uIP TCP/IP stack. */
	uip_init();
//	SendString("uip_init***\r\n");
	/* Initialize the HTTP server. */
//	httpd_init();
	tcp_server_init();
	//SendString("tcp_server_init***\r\n");
	arptimer = 0;

  Timer4Init();           //影响我们网络连接问题
//	SendString("timer4_init***\r\n");
  Timer0Init();
//	SendString("timer0_init***\r\n");
  USART3_Init();//串口2初始化
//	SendString("USART3_Init...\r\n");
	
	
	 
	 
	WDT_CONTR = 0x04;       //看门狗定时器溢出时间计算公式: (12 * 32768 * PS) / FOSC (秒)
                            //设置看门狗定时器分频数为32,溢出时间如下:
                            //11.0592M : 1.14s
                            //18.432M  : 0.68s
                            //20M      : 0.63s
    WDT_CONTR |= 0x20;      //启动看门狗
  // 	SendString("WDT_CONTR***1.14s\r\n");
  
  while(1)
	{
	    WDT_CONTR |= 0x10;  //喂狗程序
      
		/* Let the tapdev network device driver read an entire IP packet
       into the uip_buf. If it must wait for more than 0.5 seconds, it
       will return with the return value 0. If so, we know that it is
       time to call upon the uip_periodic(). Otherwise, the tapdev has
       received an IP packet that is to be processed by uIP. */
    uip_len = dev_poll();
	for(j=0;j<500;j++);
/*
	if(uip_len > 0)
	{
		printuf("--------------- uip_len = 0x%x", uip_len);
		printuf("%x ----------\r\n", uip_len);
		for(i=0;i<uip_len;i++)
		{
			printuf("%x ", uip_buf[i]);
			if((i+1)%16==0) printu("\r\n");			
		}
		printu("\r\n");			
	}
*/
    if(uip_len == 0) {
      for(i = 0; i < UIP_CONNS; i++) {
	uip_periodic(i);
	/* If the above function invocation resulted in data that
	   should be sent out on the network, the global variable
	   uip_len is set to a value > 0. */
	if(uip_len > 0) {
	  uip_arp_out();
	  dev_send();
	}
      }

#if UIP_UDP
      for(i = 0; i < UIP_UDP_CONNS; i++) {
	uip_udp_periodic(i);
	/* If the above function invocation resulted in data that
	   should be sent out on the network, the global variable
	   uip_len is set to a value > 0. */
	if(uip_len > 0) {
	  uip_arp_out();
	  dev_send();
	}
      }
#endif /* UIP_UDP */
      
      /* Call the ARP timer function every 10 seconds. */
      if(++arptimer == 20) {	
	uip_arp_timer();
	arptimer = 0;
      }
      
    } else {
      if(BUF->type == htons(UIP_ETHTYPE_IP)) {
	uip_arp_ipin();
	uip_input();
	/* If the above function invocation resulted in data that
	   should be sent out on the network, the global variable
	   uip_len is set to a value > 0. */
	if(uip_len > 0) {
	  uip_arp_out();
	  dev_send();
	}
      } else if(BUF->type == htons(UIP_ETHTYPE_ARP)) {
	uip_arp_arpin();
	/* If the above function invocation resulted in data that
	   should be sent out on the network, the global variable
	   uip_len is set to a value > 0. */	
	if(uip_len > 0) {	
	  dev_send();
	}
      }
    }
    
  }
  return 0;
}

//应该是日志信息 每条记录的信息
/*-----------------------------------------------------------------------------------
void uip_log(char *m)
{
  printf("uIP log message: %s\n", m);
}
/*-----------------------------------------------------------------------------------*/
