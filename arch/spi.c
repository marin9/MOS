#include "spi.h"
#include "gpio.h"
#include "timer.h"

#define VCC	19
#define CS	21
#define CK	20
#define SI	16
#define SO	26


static void usleep(unsigned int us){
	unsigned int t0=timer_get();
	while((timer_get()-t0)<us);
}


void spi_init(){
	gpio_mode(VCC, OUTPUT);
	gpio_mode(CS, OUTPUT);
	gpio_mode(SO, INPUT);
	gpio_mode(SI, OUTPUT);
	gpio_mode(CK, OUTPUT);

	gpio_set(VCC, HIGH);
	gpio_set(CS, HIGH);
	gpio_set(SI, LOW);
	gpio_set(CK, LOW);
}

void spi_begin(){
	gpio_set(CS, LOW);
	usleep(1);
}

void spi_end(){
	usleep(1);
	gpio_set(CS, HIGH);
}

void spi_sendByte(char byte){
	int i;
	for(i=7;i>=0;--i){
		gpio_set(CK, LOW);
		gpio_set(SI, byte&(1<<i));
		usleep(1);
		gpio_set(CK, HIGH);
		usleep(1);
	}
}

char spi_readByte(){
	int i;
	unsigned char byte=0;
	char bit[8];

	for(i=7;i>=0;--i){
		gpio_set(CK, LOW);
		usleep(1);
		bit[i]=gpio_get(SO);
		usleep(1);
		gpio_set(CK, HIGH);
		usleep(1);
	}

	gpio_set(CK, LOW);
	usleep(1);

	for(i=0;i<8;++i){
		byte=byte|((0x01&bit[i])<<i);
	}

	return byte;
}
