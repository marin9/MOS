#pragma once

void spi_init();
void spi_begin();
void spi_end();
void spi_sendByte(char byte);
char spi_readByte();


/*
flash wire colors:

		|
	... |
	|.|.|
	|.|.|
	|R|Y|
	|B|P|
	|W|G|
---------
*/

