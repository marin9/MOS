#include "uart.h"

#define PERIPHERAL_ADDR	  	0x20000000
#define UART_ADDR			(PERIPHERAL_ADDR+0x215000)
#define AUX_ENABLES			((volatile unsigned int*)(UART_ADDR+0x04))
#define AUX_MU_IO_REG		((volatile unsigned int*)(UART_ADDR+0x40))
#define AUX_MU_IER_REG		((volatile unsigned int*)(UART_ADDR+0x44))
#define AUX_MU_IIR_REG		((volatile unsigned int*)(UART_ADDR+0x48))
#define AUX_MU_LCR_REG		((volatile unsigned int*)(UART_ADDR+0x4C))
#define AUX_MU_MCR_REG		((volatile unsigned int*)(UART_ADDR+0x50))
#define AUX_MU_LSR_REG		((volatile unsigned int*)(UART_ADDR+0x54))
#define AUX_MU_MSR_REG		((volatile unsigned int*)(UART_ADDR+0x58))
#define AUX_MU_SCRATCH		((volatile unsigned int*)(UART_ADDR+0x5C))
#define AUX_MU_CNTL_REG		((volatile unsigned int*)(UART_ADDR+0x60))
#define AUX_MU_STAT_REG		((volatile unsigned int*)(UART_ADDR+0x64))
#define AUX_MU_BAUD_REG		((volatile unsigned int*)(UART_ADDR+0x68))

#define GPIO_ADDR         	(PERIPHERAL_ADDR+0x200000)
#define GPFSEL1 			((volatile unsigned int*)(GPIO_ADDR+0x04))
#define GPSET0  			((volatile unsigned int*)(GPIO_ADDR+0x1C))
#define GPCLR0  			((volatile unsigned int*)(GPIO_ADDR+0x28))
#define GPPUD				((volatile unsigned int*)(GPIO_ADDR+0x94))
#define GPPUDCLK0			((volatile unsigned int*)(GPIO_ADDR+0x98))


void uart_init(){
	int ra;
	*AUX_ENABLES=1;
    *AUX_MU_IER_REG=0;
    *AUX_MU_CNTL_REG=0;
    *AUX_MU_LCR_REG=3;
    *AUX_MU_MCR_REG=0;
    *AUX_MU_IER_REG=0;
    *AUX_MU_IIR_REG=0xC6;
    *AUX_MU_BAUD_REG=270;

    // ((250,000,000/115200)/8)-1 = 270
	// 115200, 8 bit, no parity, 1 stop bit

    ra=*GPFSEL1;
    ra&=~(7<<12); //gpio14
    ra|=2<<12;    //alt5
    ra&=~(7<<15); //gpio15
    ra|=2<<15;    //alt5
    *GPFSEL1=ra;

    *GPPUD=0;
    for(ra=0;ra<200;ra++) asm volatile ("nop");
    *GPPUDCLK0=(1<<14)|(1<<15);
    for(ra=0;ra<200;ra++) asm volatile ("nop");
    *GPPUDCLK0=0;

    *AUX_MU_CNTL_REG=3;
    uart_recv(&ra);
}

int uart_send(int c){
	if(!((*AUX_MU_LSR_REG)&0x20)){
		return 0;
	}else{
		*AUX_MU_IO_REG=(unsigned int)c;
		return 1;
	}
}

int uart_recv(int *c){
	if(!((*AUX_MU_LSR_REG)&0x01)){
		return 0;
	}else{
		*c=*AUX_MU_IO_REG;
		return 1;
	}
}
