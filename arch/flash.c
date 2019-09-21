#include "flash.h"
#include "spi.h"

#define CMD_READ	0x03
#define CMD_WRITE	0x02
#define CMD_WREN	0x06
#define CMD_WRDI	0x04
#define CMD_RDSR	0x05
#define CMD_WRSR	0x01


char flash_readSR(){
	char sr;
	spi_begin();
	spi_sendByte(CMD_RDSR);
	sr=spi_readByte();
	spi_end();
	return sr;
}

void flash_writeSR(char byte){
	while(flash_readSR()&1);

	spi_begin();
	spi_sendByte(CMD_WREN);
	spi_end();
	
	spi_begin();
	spi_sendByte(CMD_WRSR);
	spi_sendByte(byte);
	spi_end();
}

char flash_readByte(int addr){
	char addr2=(char)((addr&0xff0000)>>16);
	char addr1=(char)((addr&0xff00)>>8);
	char addr0=(char)(addr&0x00ff);
	char byte;

	while(flash_readSR()&1);

	spi_begin();
	spi_sendByte(CMD_READ);
	if(ADDR_LEN==24){
		spi_sendByte(addr2);
	}
	spi_sendByte(addr1);
	spi_sendByte(addr0);
	byte=spi_readByte();
	spi_end();
	return byte;
}

void flash_writeByte(int addr, char data){
	char addr2=(char)((addr&0xff0000)>>16);
	char addr1=(char)((addr&0xff00)>>8);
	char addr0=(char)(addr&0x00ff);

	while(flash_readSR()&1);

	spi_begin();
	spi_sendByte(CMD_WREN);
	spi_end();

	spi_begin();
	spi_sendByte(CMD_WRITE);
	if(ADDR_LEN==24){
		spi_sendByte(addr2);
	}
	spi_sendByte(addr1);
	spi_sendByte(addr0);
	spi_sendByte(data);
	spi_end();
}

int flash_readPage(int addr, char *data){
	int i;
	char addr2=(char)((addr&0xff0000)>>16);
	char addr1=(char)((addr&0xff00)>>8);
	char addr0=(char)(addr&0x00ff);

	while(flash_readSR()&1);
	
	spi_begin();
	spi_sendByte(CMD_READ);
	if(ADDR_LEN==24){
		spi_sendByte(addr2);
	}
	spi_sendByte(addr1);
	spi_sendByte(addr0);
	for(i=0;i<PAGE_LEN;++i){
		data[i]=spi_readByte();
	}
	spi_end();
	return PAGE_LEN;
}

int flash_writePage(int addr, char *data){
	int i;
	char addr2=(char)((addr&0xff0000)>>16);
	char addr1=(char)((addr&0xff00)>>8);
	char addr0=(char)(addr&0x00ff);

	while(flash_readSR()&1);

	spi_begin();
	spi_sendByte(CMD_WREN);
	spi_end();

	spi_begin();
	spi_sendByte(CMD_WRITE);
	if(ADDR_LEN==24){
		spi_sendByte(addr2);
	}
	spi_sendByte(addr1);
	spi_sendByte(addr0);
	for(i=0;i<PAGE_LEN;++i){
		spi_sendByte(data[i]);
	}
	spi_end();

	return PAGE_LEN;
}