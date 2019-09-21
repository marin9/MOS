#include "timer.h"

#define RPI_PERIPHERAL_ADDR		0x20000000
#define SYSTIMER_ADDR			(RPI_PERIPHERAL_ADDR+0x3000)
#define SYS_TIMER_CLO	((volatile unsigned int*)(SYSTIMER_ADDR+0x04))
#define SYS_TIMER_CHI	((volatile unsigned int*)(SYSTIMER_ADDR+0x08))

unsigned int timer_get(){
	return *SYS_TIMER_CLO;
}
