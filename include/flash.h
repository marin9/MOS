#pragma once

#define CAPACITY	(128*1024)
#define PAGE_LEN	256
#define ADDR_LEN	24


char flash_readSR();
void flash_writeSR(char byte);
char flash_readByte(int addr);
void flash_writeByte(int addr, char data);
int flash_readPage(int addr, char *data);
int flash_writePage(int addr, char *data);
