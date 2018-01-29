#include "enc28j60.h"
#include "spi.h"
//#include "main.h"
//#include "mcu_uart.h"

//u8_t digit[4];
//union FLAG1 flag1;
#define MIN(a,b) (a) < (b) ? (a) : (b)
idata u8_t Enc28j60Bank;
idata u16_t NextPacketPtr;

/// ENC28J60 
void delay_us(int t1)
{
	while(t1--);
}

void delay_ms(int t1)
{ 
	idata int i; 
	while(t1--) 
	{
		i=100;while(i--);
	}
}

//int  k=0;
//u16_t_u8_tS  next_packet_ptr;
//u16_t_u8_tS ip_identfier;
//u8_t  code avr_mac[6]={0x12,0x34,0x56,0x78,0x90,0xAB};	  //
//u8_t  code avr_mac[6]={UIP_ETHADDR0, UIP_ETHADDR1, UIP_ETHADDR2, UIP_ETHADDR3, UIP_ETHADDR4, UIP_ETHADDR5};	  //

//u8_t  code avr_ip[4] = {192,168,1,13};
//u8_t  code server_mac[6],server_ip[4]={172,31,220,2};

//u8_t  code avr_mac[6]={0xab,0xed,0xa1,0xfd,0xf0,0x78};	  //
//*******************************************************************************************
//
// Function : enc28j60ReadOp
// Description : 
//
//*******************************************************************************************
u8_t enc28j60ReadOp(u8_t op, u8_t address)
{
	u8_t dat1;
	// activate CS	
	CSN =0;
	// issue read command
	WriteByte(op | (address & ADDR_MASK));	
	dat1 = ReadByte();
	// do dummy read if needed (for mac and mii, see datasheet page 29)
	if(address & 0x80) 	dat1 = ReadByte();
	// release CS
	CSN=1;
	return(dat1);
}
//*******************************************************************************************
//
// Function : enc28j60WriteOp
// Description : 
//
//*******************************************************************************************
void enc28j60WriteOp(u8_t op, u8_t address, u8_t mydat)
{
	CSN=0;
	// issue write command
	WriteByte( op | (address & ADDR_MASK));
	// write data
	WriteByte(mydat);
	CSN=1;
}
//*******************************************************************************************
//
// Function : icmp_send_request
// Description : Send ARP request packet to destination.
//
//*******************************************************************************************
void enc28j60SetBank(u8_t address)
{
	// set the bank (if needed)
	if((address & BANK_MASK) != Enc28j60Bank)
	{
		// set the bank
		enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, (ECON1_BSEL1|ECON1_BSEL0));
		enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, (address & BANK_MASK)>>5);
		Enc28j60Bank = (address & BANK_MASK);
	}
}
//*******************************************************************************************
//
// Function : icmp_send_request
// Description : Send ARP request packet to destination.
//
//*******************************************************************************************
u8_t enc28j60Read(u8_t address)
{
	// select bank to read
	enc28j60SetBank(address);	
	// do the read
	return enc28j60ReadOp(ENC28J60_READ_CTRL_REG, address);
}
//*******************************************************************************************
//
// Function : icmp_send_request
// Description : Send ARP request packet to destination.
//
//*******************************************************************************************
void enc28j60Write(u8_t address, u8_t mydat)
{
	// select bank to write
	enc28j60SetBank(address);

	// do the write
	enc28j60WriteOp(ENC28J60_WRITE_CTRL_REG, address, mydat);
}
//*******************************************************************************************
//
// Function : icmp_send_request
// Description : Send ARP request packet to destination.
//
//*******************************************************************************************
/*
u16_t enc28j60_read_phyreg(u8_t address)
{
	u16_t mydat;
	
	// set the PHY register address
	enc28j60Write(MIREGADR, address);
	enc28j60Write(MICMD, MICMD_MIIRD);
	
	// Loop to wait until the PHY register has been read through the MII
	// This requires 10.24us
	while( (enc28j60Read(MISTAT) & MISTAT_BUSY) );
	
	// Stop reading
	enc28j60Write(MICMD, MICMD_MIIRD);
	
	// Obtain results and return
	mydat = enc28j60Read ( MIRDL );
	mydat |= enc28j60Read ( MIRDH );

	return mydat;
}

*/

//*******************************************************************************************
//
// Function : icmp_send_request
// Description : Send ARP request packet to destination.
//
//*******************************************************************************************
void enc28j60PhyWrite(u8_t address, u16_t mydat)
{
	// set the PHY register address
	enc28j60Write(MIREGADR, address);
	// write the PHY data
	enc28j60Write(MIWRL, mydat & 0x00ff);
	enc28j60Write(MIWRH, mydat >> 8);
	// wait until the PHY write completes
	while(enc28j60Read(MISTAT) & MISTAT_BUSY)
	{
		delay_us(15);
	}
}

void enc28j60ReadBuffer(u16_t len, u8_t* dat)
{
    // assert CS
//	ENC28J60_CONTROL_PORT &= ~(1<<ENC28J60_CONTROL_CS);
    CSN = 0;
    // issue read command
	//SPDR = ENC28J60_READ_BUF_MEM;
	WriteByte(ENC28J60_READ_BUF_MEM);
    //while(!(SPSR & (1<<SPIF)));
    while(len--)
    {
        // read data
		//SPDR = 0x00;
        //while(!(SPSR & (1<<SPIF)));
        //*dat++ = SPDR;
		*dat++ = ReadByte();
    }	
    // release CS
	//ENC28J60_CONTROL_PORT |= (1<<ENC28J60_CONTROL_CS);
	CSN = 1;
}

void enc28j60WriteBuffer(u16_t len, u8_t* dat)
{
    // assert CS
	//ENC28J60_CONTROL_PORT &= ~(1<<ENC28J60_CONTROL_CS);
	CSN = 0;
    
    // issue write command
	//SPDR = ENC28J60_WRITE_BUF_MEM;
	WriteByte(ENC28J60_WRITE_BUF_MEM);
   // while(!(SPSR & (1<<SPIF)));
    while(len--)
    {
        // write data
		//SPDR = *dat++;
        //while(!(SPSR & (1<<SPIF)));
		WriteByte(*dat++);
    }	
    // release CS
//	ENC28J60_CONTROL_PORT |= (1<<ENC28J60_CONTROL_CS);
	CSN = 1;
}


#define ETHERNET_MIN_PACKET_LENGTH	0x3C
#define ETHERNET_HEADER_LENGTH		0x0E

#define IP_TCP_HEADER_LENGTH 40
#define TOTAL_HEADER_LENGTH (IP_TCP_HEADER_LENGTH+ETHERNET_HEADER_LENGTH)


void enc28j60PacketSend(u16_t len, u8_t* packet)
{

/*
    // dump packet
    int i,j;
    for (i=0;i<len;i+=16)
    {
        printf("%04x  ",i);
        for (j=0;j<16;j++)
            printf("%02x ",(i+j) < len ? packet[i+j] : 0);
        printf("  ");
        for (j=0;j<16;j++)
            printf("%c", (i+j) < len ? isprint(packet[i+j]) ? packet[i+j] : '.' : '.');
         printf("\n");
    }
*/

    // Set the write pointer to start of transmit buffer area
	enc28j60Write(EWRPTL, TXSTART_INIT);
    enc28j60Write(EWRPTH, TXSTART_INIT>>8);

    // Set the TXND pointer to correspond to the packet size given
	enc28j60Write(ETXNDL, (TXSTART_INIT+len));
    enc28j60Write(ETXNDH, (TXSTART_INIT+len)>>8);

    // write per-packet control byte
	enc28j60WriteOp(ENC28J60_WRITE_BUF_MEM, 0, 0x00);

    // TODO, fix this up

    if( uip_len <= TOTAL_HEADER_LENGTH )
    {
        // copy the packet into the transmit buffer
		enc28j60WriteBuffer(len, packet);
    }
    else
    {
          len -= TOTAL_HEADER_LENGTH;
        enc28j60WriteBuffer(TOTAL_HEADER_LENGTH, packet);
        enc28j60WriteBuffer(len, (unsigned char *)uip_appdata);
    }



    //
	// Errata workaround #12, reset TX logic
	//
	{
        // clear TXRST bit in ECON1
		//enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRST);
		//delay(100);
		// set TXRST bit in ECON1
		//enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRST);
	}
    // send the contents of the transmit buffer onto the network
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);
}




u16_t enc28j60PacketReceive(u16_t maxlen, u8_t* packet)
{
    u16_t rxstat;
    u16_t len;

    // check if a packet has been received and buffered
	if( !(enc28j60Read(EIR) & EIR_PKTIF) )
    {
        // Errata workaround #6, PKTIF is not reliable
		// double check by looking at EPKTCNT
		if (enc28j60Read(EPKTCNT) == 0)
            return 0;
    }
    
    // Make absolutely certain that any previous packet was discarded	
	//if( WasDiscarded == FALSE)
	//	MACDiscardRx();

    // Set the read pointer to the start of the received packet
	enc28j60Write(ERDPTL, (NextPacketPtr));
    enc28j60Write(ERDPTH, (NextPacketPtr)>>8);

    // read the next packet pointer
	NextPacketPtr  = enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0);
    NextPacketPtr |= enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0)<<8;

    // read the packet length
	len  = enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0);
    len |= enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0)<<8;

    // read the receive status
	rxstat  = enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0);
    rxstat |= enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0)<<8;

    // limit retrieve length
	// (we reduce the MAC-reported length by 4 to remove the CRC)
	len = MIN(len, maxlen);

    // copy the packet from the receive buffer
	enc28j60ReadBuffer(len, packet);

    // Move the RX read pointer to the start of the next received packet
	// This frees the memory we just read out
/*
    enc28j60Write(ERXRDPTL, (NextPacketPtr));
    enc28j60Write(ERXRDPTH, (NextPacketPtr)>>8);
*/
    // Errata workaround #13. Make sure ERXRDPT is odd
	//
	{
        u16_t rs,re;
        rs = enc28j60Read(ERXSTH);
        rs <<= 8;
        rs |= enc28j60Read(ERXSTL);
        re = enc28j60Read(ERXNDH);
        re <<= 8;
        re |= enc28j60Read(ERXNDL);
        if (NextPacketPtr - 1 < rs || NextPacketPtr - 1 > re)
        {
            enc28j60Write(ERXRDPTL, (re));
            enc28j60Write(ERXRDPTH, (re)>>8);
        }
        else
        {
            enc28j60Write(ERXRDPTL, (NextPacketPtr-1));
            enc28j60Write(ERXRDPTH, (NextPacketPtr-1)>>8);
        }
    }

    // decrement the packet counter indicate we are done with this packet
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);

    return len;
}

void dev_init(void)
{
    enc28j60_init();
}

void dev_send(void)
{
    enc28j60PacketSend(uip_len, uip_buf);
}

u16_t dev_poll(void)
{
    return enc28j60PacketReceive(UIP_BUFSIZE, uip_buf);
}

void enc28j60_init(void)
{


    // perform system reset
	enc28j60WriteOp(ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET);
 //   delay(50);
    // check CLKRDY bit to see if reset is complete
	//while(!(enc28j60Read(ESTAT) & ESTAT_CLKRDY));
	// Errata workaround #2, CLKRDY check is unreliable, delay 1 mS instead
	delay_ms(5);

    // lamp test
//	enc28j60PhyWrite(PHLCON, 0x0AA2);



    // do bank 0 stuff
	// initialize receive buffer
	// 16-bit transfers, must write low byte first

    // set receive buffer start address
	NextPacketPtr = RXSTART_INIT;
    enc28j60Write(ERXSTL, RXSTART_INIT&0xFF);
    enc28j60Write(ERXSTH, RXSTART_INIT>>8);

    // set receive pointer address
	enc28j60Write(ERXRDPTL, RXSTART_INIT&0xFF);
    enc28j60Write(ERXRDPTH, RXSTART_INIT>>8);

    // set receive buffer end
	// ERXND defaults to 0x1FFF (end of ram)
	enc28j60Write(ERXNDL, RXSTOP_INIT&0xFF);
    enc28j60Write(ERXNDH, RXSTOP_INIT>>8);

    // set transmit buffer start
	// ETXST defaults to 0x0000 (beginnging of ram)
	enc28j60Write(ETXSTL, TXSTART_INIT&0xFF);
    enc28j60Write(ETXSTH, TXSTART_INIT>>8);

    // do bank 2 stuff
	// enable MAC receive
	enc28j60Write(MACON1, MACON1_MARXEN|MACON1_TXPAUS|MACON1_RXPAUS);
    // bring MAC out of reset
	enc28j60Write(MACON2, 0x00);
    // enable automatic padding and CRC operations
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, MACON3, MACON3_PADCFG0|MACON3_TXCRCEN|MACON3_FRMLNEN);
//	enc28j60Write(MACON3, MACON3_PADCFG0|MACON3_TXCRCEN|MACON3_FRMLNEN);
	// set inter-frame gap (non-back-to-back)
	enc28j60Write(MAIPGL, 0x12);
    enc28j60Write(MAIPGH, 0x0C);
    // set inter-frame gap (back-to-back)
	enc28j60Write(MABBIPG, 0x12);
    // Set the maximum packet size which the controller will accept
	enc28j60Write(MAMXFLL, MAX_FRAMELEN&0xFF);	
    enc28j60Write(MAMXFLH, MAX_FRAMELEN>>8);

    // do bank 3 stuff
	// write MAC address
	// NOTE: MAC address in ENC28J60 is byte-backward
	enc28j60Write(MAADR5, UIP_ETHADDR0);
	enc28j60Write(MAADR4, UIP_ETHADDR1);
	enc28j60Write(MAADR3, UIP_ETHADDR2);
	enc28j60Write(MAADR2, UIP_ETHADDR3);
	enc28j60Write(MAADR1, UIP_ETHADDR4);
	enc28j60Write(MAADR0, UIP_ETHADDR5);

    // no loopback of transmitted frames
	enc28j60PhyWrite(PHCON2, PHCON2_HDLDIS);

    // switch to bank 0
	enc28j60SetBank(ECON1);
    // enable interrutps
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE|EIE_PKTIE);
    // enable packet reception
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);
/*
    enc28j60PhyWrite(PHLCON, 0x0AA2);

    // setup duplex ----------------------

    // Disable receive logic and abort any packets currently being transmitted
    enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRTS|ECON1_RXEN);
    
    {
        uint16_t temp;
        // Set the PHY to the proper duplex mode
        temp = enc28j60PhyRead(PHCON1);
        temp &= ~PHCON1_PDPXMD;
        enc28j60PhyWrite(PHCON1, temp);
        // Set the MAC to the proper duplex mode
        temp = enc28j60Read(MACON3);
        temp &= ~MACON3_FULDPX;
        enc28j60Write(MACON3, temp);
    }

    // Set the back-to-back inter-packet gap time to IEEE specified 
    // requirements.  The meaning of the MABBIPG value changes with the duplex
    // state, so it must be updated in this function.
    // In full duplex, 0x15 represents 9.6us; 0x12 is 9.6us in half duplex
    //enc28j60Write(MABBIPG, DuplexState ? 0x15 : 0x12);	
    
    // Reenable receive logic
    enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);

    // setup duplex ----------------------
*/
}


